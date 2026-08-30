# Handling Textures

Status: documentation of the current implementation
Last verified: 2026-08-30

This document describes how SuperEZ imports, creates, uploads, views, binds, samples,
transitions, and destroys textures. It is intended to serve as the current functional
specification: statements under **Current behavior** describe what the engine does today,
while **Future requirements** records proposed behavior that is not yet implemented.

For the scene files and image formats that supply object textures, see the
[Asset Import Pipeline](asset-import-pipeline.md).

## 1. Scope

This specification covers:

- scene texture references and AssetSuite image decoding;
- D3D12 texture-resource creation and ownership;
- CPU conversion, mip generation, upload buffers, and copy submission;
- raw and sRGB shader-resource views;
- material indirection and graphics texture binding;
- render-target, depth, compute, and swap-chain textures where they share the same
  infrastructure;
- sampler creation, shader sampling, resource-state tracking, and scene lifetime;
- current behavior for missing, invalid, or procedurally generated texture data.

Geometry texture-coordinate streams are discussed only where they determine texture
sampling. General material models and render-graph scheduling are outside this document.

## 2. Terms

| Term | Meaning in SuperEZ |
|---|---|
| Texture resource | A D3D12 `ID3D12Resource` stored in `RenderContext::textures`. |
| Texture handle | An `HTexture`; depending on the API, its index currently refers either to `textures` or to `materials`. |
| Material handle | The `HTexture` returned by `CreateTexture`; its index refers to `RenderContext::materials`, despite its type name. |
| SRV | Shader-resource view used to read a texture from a shader. |
| UAV | Unordered-access view used for compute-shader writes. |
| RTV | Render-target view used for graphics output. |
| DSV | Depth-stencil view used for depth output. |
| Raw view | An `R8G8B8A8_UNORM` SRV that returns stored channel values without sRGB decoding. |
| sRGB view | An `R8G8B8A8_UNORM_SRGB` SRV that decodes RGB channels to linear values on sampling. |
| Static descriptor | A descriptor allocated from the first 64 entries of a descriptor heap. |
| Dynamic descriptor | A descriptor allocated after the 64-entry static partition; "dynamic" describes allocation policy, not per-frame reclamation. |

The most important current distinction is that one C++ handle type represents two
different index spaces. Resource-oriented APIs such as `TransitionTo`, `GetTexture`, and
`BindTextureSRV` interpret `HTexture` as an index into `textures`. Scene rendering passes
the result of `CreateTexture` to `BindTexture`, which interprets the same type as an index
into `materials`.

## 3. Current behavior

### 3.1 Scene declaration and image decoding

Every `<GameObject>` may provide a `texture` attribute. When it is absent,
`Engine::ProcessGameObjects` substitutes the literal filename `default_texture`; this is
not a built-in fallback asset.

During `Engine::LoadAssets`, the path is resolved relative to the current scene folder.
AssetSuite chooses a decoder automatically from the extension and currently recognizes
PNG, BMP, and PPM. The engine requests `AssetSuite::OutputFormat::RGB8`, so decoded source
alpha is not carried into the renderer. `PrepareTextureForUpload` expands each RGB texel
to RGBA and writes alpha as 255.

The return values from `ImageLoadAndDecode` and `ImageGet` are currently ignored. Unlike
mesh extraction, a failed image operation does not skip the game object or produce a
specific engine diagnostic. The image output and descriptor objects are reused across the
object loop, so a failure can also leave empty or stale data available to subsequent
texture creation.

The scene loader creates one GPU texture and one `Material` for every game object. It does
not cache by source filename, mesh, decoded contents, or image hash. Multiple objects that
reference the same file therefore receive duplicate resources, descriptors, upload
buffers, and material entries.

See [`Engine::LoadAssets`](../source/engine/engine/Engine.cpp) and AssetSuite's
[`AssetSuite.h`](../source/externals/AssetSuite/inc/AssetSuite.h).

### 3.2 Scene texture descriptor

The scene-loading lambda constructs this effective `TextureCreateDesc`:

| Field | Current scene value |
|---|---|
| Width and height | AssetSuite's decoded image dimensions |
| Resource format | `DXGI_FORMAT_R8G8B8A8_TYPELESS` |
| Raw SRV format | `DXGI_FORMAT_R8G8B8A8_UNORM` |
| sRGB SRV format | `DXGI_FORMAT_R8G8B8A8_UNORM_SRGB` when color-pipeline debugging is enabled |
| Mip count | Full chain down to 1 x 1 |
| Initial state | `D3D12_RESOURCE_STATE_COMMON` |
| SRV allocation | Dynamic partition |
| Lifetime | `SCENE` |
| Name | `TEX_<meshName>` |

`ENABLE_COLOR_PIPELINE_DEBUG` is currently `1`. In that configuration both raw and sRGB
SRVs are created for the same typeless resource. If it is set to `0`, only an sRGB SRV is
created through the primary `srvFormat` field.

`TextureCreateDesc` also supports single-mip resources, explicit mip counts, alternate
resource and view formats, initial state, heap flags, optional SRV/UAV/RTV/DSV intent,
static descriptor allocation, and `APP` versus `SCENE` lifetime. Only a subset of those
fields is validated by the creation code.

### 3.3 Resource and view creation

`RenderContext::CreateTextureResource`:

1. combines the requested UAV, RTV, and DSV flags into the D3D12 resource flags;
2. creates a 2D, single-array-slice, single-sample texture with the requested dimensions,
   format, mip count, initial state, and heap flags;
3. creates the primary SRV when `createSrv` is true;
4. appends a `Texture` wrapper to `RenderContext::textures`;
5. optionally creates a second sRGB SRV and records its descriptor index;
6. optionally creates a mip-zero UAV and records its descriptor index.

SRVs expose every requested mip. UAV creation exposes only mip zero. RTVs and DSVs are
not created by this function even though their resource flags are selected here;
`CreateRenderTarget` and `CreateDepthBuffer` create those descriptors separately.

The `Texture` wrapper stores width, height, mip count, resource pointer, debug name,
current and previous states, lifetime, and descriptor indices. It owns and releases the
D3D12 resource. It does not own descriptor slots. See
[`Texture`](../source/engine/core/Texture.h) and
[`RenderContext::CreateTextureResource`](../source/engine/renderer/RenderContext.cpp).

### 3.4 CPU preparation and mip generation

`RenderContext::CreateTexture` first creates the default-heap texture resource and asks
D3D12 for placed subresource footprints covering the full mip chain. One upload-heap
buffer large enough for all footprints is then created.

For mip zero, `PrepareTextureForUpload` reads three bytes per source texel in RGB order,
packs them into an `R8G8B8A8`-compatible 32-bit value, and forces alpha to 255. For each
later mip, `PrepareAndDonwsampleTexture` box-filters the immediately preceding mip. The
source interval for each destination texel is derived by integer scaling, so odd and
non-power-of-two dimensions are supported until both dimensions reach one.

When the texture has an sRGB view, RGB values are decoded to linear light before averaging
and encoded back to sRGB bytes afterward. Alpha is averaged directly. Consequently, the
current scene mip chain is generated in linear light even when the forward shader is put
into the raw-view comparison mode.

If `CreateTexture` receives a null data pointer, every mip is filled with a distinct solid
debug color from `GenerateTextureForUpload`; it does not produce a conventional missing
texture checkerboard. The separate [`TextureGenerator`](../source/engine/core/TextureGenerator.h)
can create RGBA textures, checkerboards, gradients, and mip chains, but it is not connected
to the current scene-loading or `RenderContext::CreateTexture` path.

### 3.5 Upload and submission

Each mip is uploaded separately:

1. the target footprint is mapped within the shared upload buffer;
2. rows are copied using D3D12's footprint row pitch;
3. a new command list is created and reset;
4. the whole texture is transitioned to `COPY_DEST`;
5. the selected upload footprint is copied to the selected texture subresource;
6. the whole texture is transitioned to `PIXEL_SHADER_RESOURCE`;
7. the command list is closed and immediately submitted.

This produces one command list and one queue submission per mip. There is no explicit
flush or fence wait inside `CreateTexture`; ordering on the same command queue makes the
successive transitions and copies execute in submission order. The upload buffer remains
stored in `RenderContext::buffers` after upload.

After all mips are submitted, a `Material` is created with the resource's raw and sRGB SRV
indices. `CreateTexture` returns an `HTexture` whose numeric index is the new entry in
`materials`, not the texture resource's entry in `textures`.

### 3.6 Descriptors and sampler

The CBV/SRV/UAV heap has 1,024 entries: 64 static entries followed by 960 dynamic entries.
Scene textures allocate their SRVs dynamically. Render targets, depth textures, and most
compute intermediates request static SRVs; compute outputs generally request static UAVs.

`DescriptorHeap::Reset` only sets the dynamic allocation count to zero. It does not clear
descriptors. The static count is not reset by that method. Descriptor allocation is linear;
`Free` is currently empty.

The sampler heap contains one default sampler created at engine initialization:

- anisotropic filtering with maximum anisotropy 16;
- wrap addressing on U, V, and W;
- full LOD range, zero mip bias;
- comparison function `ALWAYS`.

There is no per-texture sampler selection. Graphics binding always uses the sampler heap's
first GPU descriptor. See [`DescriptorHeap`](../source/engine/bind/DescriptorHeap.h) and
[`RenderContext::CreateDefaultSamplers`](../source/engine/renderer/RenderContext.cpp).

### 3.7 Material binding and forward sampling

`MaterialComponent` and `RenderItem` store the material-table `HTexture` returned by
`CreateTexture`. `RenderContext::BindTexture` validates that index against `materials`,
selects either the raw or sRGB descriptor, and binds two adjacent root parameters:

```cpp
SetGraphicsRootDescriptorTable(slot, textureSrv);
SetGraphicsRootDescriptorTable(slot + 1, defaultSampler);
```

The forward pass calls `BindTexture(commandList, item.texture, 3)`. In its root signature,
parameter 3 is SRV table `t0` and parameter 4 is sampler table `s0`. The selection pass
also binds the material this way, although its current pixel shader does not sample the
declared texture.

The forward pixel shader flips the V coordinate before sampling:

```hlsl
float2 uv = float2(input.texCoord.x, 1.0f - input.texCoord.y);
```

Normal sampling is implicit-derivative anisotropic sampling. Debug settings can instead
apply a shader mip bias or force an explicit mip level, and can tint the result according
to the selected level.

With color-pipeline debugging enabled, `ForwardPass::useLinearLighting` controls both
descriptor selection and final output encoding. When enabled, the sRGB SRV decodes albedo
to linear values, lighting runs in linear space, and `CompositionPass` encodes RGB back to
sRGB-like values. When disabled, the raw view is lit and composition leaves the values
unchanged. See [`ForwardPass`](../source/engine/renderer/passes/ForwardPass.cpp),
[`shaders.hlsl`](../source/engine/renderer/shaders/shaders.hlsl), and
[`composition.hlsl`](../source/engine/renderer/shaders/composition.hlsl).

### 3.8 Render targets, depth textures, compute textures, and back buffers

All renderer-owned 2D resources use the same `Texture` wrapper and `textures` collection:

- color render targets create an SRV-capable texture plus a separate RTV;
- depth buffers use an `R32_TYPELESS` resource, `R32_FLOAT` SRV, and `D32_FLOAT` DSV;
- compute intermediates opt into UAV and sometimes RTV access through
  `TextureCreateDesc`;
- swap-chain back buffers are wrapped as textures but receive no valid SRV;
- a `RenderTarget` or `DepthBuffer` stores the numeric index of its underlying texture.

Graphics resource reads use `BindTextureSRV`, which interprets `HTexture` as a resource
handle. Compute reads and writes use `BindTextureOnlySRV` and `BindTextureOnlyUAV`.
Unlike `BindTexture`, these methods do not bind a sampler and do not use `materials`.

Render-pass textures are commonly found by their debug name through `GetTexture(const
char*)`. A missing name returns an invalid handle. Callers generally assume the named
texture exists before indexing it.

### 3.9 Resource-state tracking

Each `Texture` tracks one current state and one previous state for the whole resource.
`TransitionTo` emits an all-subresources transition only when the requested state differs
from the recorded current state, then updates the two stored states. `TransitionBack`
transitions to the single recorded previous state.

This model supports the renderer's mostly serial, whole-resource usage. It does not track
states per mip or array slice, and `TransitionBack` is not a stack: nested or interleaved
transitions can overwrite the state a caller expected to restore.

### 3.10 Lifetime and scene unload

Most pass-owned render targets, depth textures, and compute intermediates use `APP`
lifetime. Scene image textures default to `SCENE` lifetime.

`UnloadAssets` deletes every `SCENE` texture object but does not remove its pointer from
the `textures` vector. It then deletes all materials and resets the CBV/SRV/UAV dynamic
descriptor count. Texture upload buffers are not deleted there, and render targets and
depth buffers are not rebuilt when the RTV and DSV allocation counts are reset.

The `RenderContext` destructor currently deletes pipeline states, shaders, root
signatures, and cameras, but it does not delete the remaining texture, material, buffer,
render-target, depth-buffer, command-list, mesh, vertex-buffer, or descriptor-heap
objects. Texture lifetime is therefore not consistently closed at shutdown.

## 4. Required invariants

Correct texture use currently depends on all of the following:

1. Every visible scene game object resolves to a decodable image with nonzero dimensions
   and at least `width * height * 3` RGB bytes.
2. Scene albedo resources use a typeless format when both raw and sRGB SRVs are created.
3. An SRV, UAV, RTV, or DSV format is compatible with the underlying resource format.
4. Every requested mip fits the resource's declared mip count and upload footprints.
5. A material-table handle is passed only to `BindTexture`; a resource handle is passed
   only to texture-resource APIs.
6. The graphics root signature places the sampler table immediately after the texture SRV
   table when calling `BindTexture`.
7. The command list has the matching CBV/SRV/UAV and sampler heaps bound before graphics
   texture binding, or the CBV/SRV/UAV heap bound before compute texture binding.
8. A texture is in a shader-resource state before shader reads, UAV state before compute
   writes, render-target state before RTV writes, depth-write state before DSV writes, and
   the proper copy state before copies.
9. Dynamic descriptor allocations remain within the 960-entry dynamic partition and
   static allocations remain within the 64-entry static partition.
10. Render-pass texture names are unique when looked up through `GetTexture(const char*)`.

Several of these invariants are implicit or enforced only by assertions. The engine does
not validate the full descriptor/resource contract or distinguish the two handle domains
at the type level.

## 5. Missing, optional, and generated textures

### 5.1 Missing scene texture file

There is no explicit untextured scene path and no documented fallback filename. A missing
or unsupported file is passed through the same unchecked decode path. Depending on the
reused AssetSuite outputs, the result may be empty, stale, or otherwise unsuitable for the
requested descriptor. It is not guaranteed to become a safe placeholder texture.

### 5.2 Game object without a `texture` attribute

The loader substitutes `default_texture` as a relative filename. Unless that file exists
and AssetSuite recognizes it, this behaves like any other failed image load. Absence is
not represented by `HTexture::Invalid()` for renderable scene objects.

### 5.3 Null data passed directly to `CreateTexture`

This path is supported by the renderer. Each mip is filled with a different diagnostic
solid color. The caller must still provide valid dimensions, format, and mip count.

### 5.4 Empty pass-owned texture

`CreateEmptyTexture` creates only the resource and requested views. It performs no upload
or clear. The pass must transition and initialize the resource before depending on its
contents.

### 5.5 Texture present but not sampled

Binding an SRV to a root signature whose active shader does not read it is legal. The
selection pass currently binds each object's material even though `selection.hlsl` returns
only the object ID.

## 6. Known limitations and risks

- `HTexture` conflates material-table indices and texture-resource indices, allowing valid
  but semantically wrong handles to reach the wrong vector.
- Image decode and conversion errors are ignored, and reused output variables make failure
  behavior nondeterministic across multiple game objects.
- Source alpha is discarded because scene images are requested as RGB8 and upload forces
  alpha to 255; alpha-tested and translucent materials are not supported by this path.
- Textures are duplicated per game object, even when objects reference the same file.
- One command list and queue submission are created per mip rather than batching the
  complete upload.
- Upload buffers remain resident after texture creation and are not reclaimed on scene
  unload.
- Scene unload deletes texture objects without erasing or nulling vector entries, leaving
  dangling pointers and preventing safe index reuse.
- Resetting descriptor counters can overwrite descriptors still referenced by app-lifetime
  resources or retained wrappers if allocation order changes.
- The descriptor allocator has no functioning free path and relies on fixed capacities.
- State tracking is whole-resource and single-previous-state only; it is not safe for
  subresource transitions or nested restoration.
- UAV creation always targets mip zero and there is no array, cube-map, 3D texture, or
  multisample texture abstraction.
- `Texture` stores width and height but exposes no accessors for them; callers query the
  D3D12 resource description instead.
- Fixed 32-character debug-name storage truncates creation names and can create ambiguous
  name lookup.
- `GetTexture(const char*)` returns an invalid handle on failure, but most callers do not
  check it before use.
- The default sampler is global: wrap addressing and 16x anisotropy cannot be selected per
  material or texture role. The shadow map also uses this wrap, non-comparison sampler.
- `TextureGenerator` duplicates some conversion and mip functionality but is not integrated
  with the actual GPU upload path; its mip averaging is not sRGB-aware.
- Render-target format `R32_UINT` currently maps to an `R32_FLOAT` resource, with selection
  code relying on raw bit interpretation during readback and debug visualization.

## 7. Future requirements: explicit texture resources and materials

This section is a proposed extension, not a description of current behavior.

1. Introduce distinct handle types for GPU texture resources, texture views, samplers, and
   materials. No API should infer the index domain from the function called.
2. Make image loading transactional: validate AssetSuite return codes, dimensions, byte
   counts, and format before allocating GPU resources.
3. Provide a named missing-texture asset and a deliberate untextured/material-default path.
4. Cache decoded images and GPU resources by canonical asset identity, while allowing
   multiple materials or view interpretations to share one resource.
5. Preserve RGBA input when available and define alpha mode explicitly.
6. Batch every mip copy for one texture into a single upload command list, retain the upload
   allocation until a fence completes, and then reclaim it.
7. Replace append-only descriptor allocation with lifetime-aware allocation and safe reuse;
   reserve stable descriptors for persistent render-graph resources.
8. Track resource state per subresource where needed, or make whole-resource state an
   explicit restriction. Replace `TransitionBack` with planned transitions rather than a
   one-entry history.
9. Make sampler choice part of material or texture binding, including clamp, border,
   comparison, anisotropy, and LOD policy.
10. Treat color space as asset metadata and view policy rather than a global debug toggle;
    color textures, data textures, normals, masks, and depth must select appropriate
    formats and mip filters.
11. Extend creation and views to arrays, cube maps, additional formats, and per-mip UAVs as
    renderer requirements grow.
12. Make scene unload erase or recycle every owned resource, material, descriptor, and
    upload allocation without invalidating app-lifetime resources.
13. Add validation that the root signature, descriptor type, shader register, resource
    format, and resource state agree before submission in debug builds.

## 8. Acceptance scenarios for a future implementation

### Shared albedo texture

- Given two game objects that reference the same canonical image asset,
- the image is decoded and uploaded once,
- both materials refer to the shared resource and appropriate sRGB view,
- and unloading either object does not invalidate the other.

### Missing image

- Given a scene texture path that does not exist or cannot be decoded,
- loading reports the object name, requested path, and AssetSuite error,
- and policy either rejects the object or binds the documented missing texture without
  reading stale output.

### Linear color sampling

- Given a known sRGB test pattern and its generated mip chain,
- the shader receives linear-light samples through the sRGB view,
- mip averages match a CPU reference within tolerance,
- and final output encoding produces the expected display values.

### Non-color data texture

- Given a normal, mask, ID, or depth texture,
- its material or pass selects a non-sRGB view and suitable mip/filter policy,
- and no global color setting changes its interpretation.

### Batched mip upload

- Given a non-power-of-two image with a full mip chain,
- one upload submission initializes every declared subresource,
- the upload allocation is retained until its fence completes and then reclaimed,
- and the final resource enters its declared read state.

### Scene reload

- Given repeated unload/load cycles with different scene sizes,
- all scene textures, materials, descriptors, and upload allocations are reclaimed or
  reused safely,
- app-lifetime render resources retain valid descriptors,
- and memory and descriptor counts remain bounded.

### Handle-domain misuse

- Given code that passes a material handle to a resource transition or a resource handle
  to material binding,
- compilation fails because the handle types are distinct.

## 9. Implementation map

| Responsibility | Current location |
|---|---|
| Scene XML texture attribute and path resolution | [`Engine.cpp`](../source/engine/engine/Engine.cpp) |
| Supported image decoders and output formats | [`AssetSuite.h`](../source/externals/AssetSuite/inc/AssetSuite.h) |
| Texture creation descriptor and renderer API | [`RenderContext.h`](../source/engine/renderer/RenderContext.h) |
| Resource, view, mip, upload, binding, and transition implementation | [`RenderContext.cpp`](../source/engine/renderer/RenderContext.cpp) |
| Texture resource ownership and state metadata | [`Texture.h`](../source/engine/core/Texture.h) |
| Procedural CPU texture helpers not used by scene loading | [`TextureGenerator.h`](../source/engine/core/TextureGenerator.h) |
| Material SRV indirection | [`Material.h`](../source/engine/asset/Material.h) |
| Typed handle representation | [`Handle.h`](../source/engine/asset/Handle.h) |
| ECS material data and saved texture filename | [`Components.h`](../source/engine/engine/Components.h) |
| Render-item material handle | [`RenderItem.h`](../source/engine/renderer/RenderItem.h) |
| Descriptor heap partitioning and allocation | [`DescriptorHeap.cpp`](../source/engine/bind/DescriptorHeap.cpp) |
| Root-signature descriptor tables | [`RootSignatureBuilder.cpp`](../source/engine/bind/RootSignatureBuilder.cpp) |
| Default albedo binding and mip controls | [`ForwardPass.cpp`](../source/engine/renderer/passes/ForwardPass.cpp) |
| Albedo, mip selection, UV flip, and lighting | [`shaders.hlsl`](../source/engine/renderer/shaders/shaders.hlsl) |
| Final linear-to-sRGB-like encoding | [`composition.hlsl`](../source/engine/renderer/shaders/composition.hlsl) |
| Render-target/depth inspection example | [`BlitPass.cpp`](../source/engine/renderer/passes/BlitPass.cpp) |
