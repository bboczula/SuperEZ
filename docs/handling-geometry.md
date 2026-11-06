# Handling Geometry

Status: documentation of the current implementation
Last verified: 2026-08-29

This document describes how SuperEZ imports, stores, binds, and draws geometry. It is
intended to serve as the current functional specification: statements under **Current
behavior** describe what the engine does today, while **Future requirements** records
proposed behavior that is not yet implemented.

For the file formats and scene rules that produce geometry, see the
[Asset Import Pipeline](asset-import-pipeline.md).

## 1. Scope

This specification covers:

- geometry streams returned by AssetSuite;
- creation and ownership of Direct3D 12 vertex buffers;
- construction of a renderable `Mesh`;
- the relationship between vertex streams, input slots, input layouts, and shaders;
- geometry binding and non-indexed drawing;
- the current behavior when a stream is not required or is unavailable.

Textures, materials, object transforms, and render-target management are only discussed
where they interact directly with geometry.

## 2. Terms

| Term | Meaning in SuperEZ |
|---|---|
| Vertex stream | One homogeneous array of per-vertex attributes, such as positions or normals. |
| Vertex buffer | A D3D12 resource containing one vertex stream. |
| Vertex-buffer view | The resource address, byte size, and stride supplied to the D3D12 input assembler. |
| Input slot | The numbered D3D12 binding point from which an input-layout element reads. |
| Input layout | The pipeline-state declaration mapping shader semantics to formats and input slots. |
| Mesh | Four vertex-buffer views plus a vertex count and local bounds. |
| Render item | A scene object that refers to a mesh and adds identity, transform, texture, and material data. |

The current representation is a **multi-stream** or **structure-of-arrays** layout. It is
not an interleaved vertex structure. Position, color, texture coordinate, and normal data
reside in separate resources.

## 3. Current behavior

### 3.1 Supported streams and fixed slot contract

The full forward-rendering path uses the following contract:

| Input slot | `VertexStream` | Shader semantic | DXGI format | Components | View stride |
|---:|---|---|---|---:|---:|
| 0 | `Position` | `POSITION` | `R32G32B32A32_FLOAT` | 4 floats | 16 bytes |
| 1 | `Color` | `COLOR` | `R32G32B32A32_FLOAT` | 4 floats | 16 bytes |
| 2 | `TexCoord` | `TEXCOORD` | `R32G32_FLOAT` | 2 floats | 8 bytes |
| 3 | `Normal` | `NORMAL` | `R32G32B32A32_FLOAT` | 4 floats | 16 bytes |

`Tangent` is recognized by `InputLayout` as a four-float `TANGENT` element, but meshes do
not store or bind a tangent stream. Tangents are therefore not usable by the current mesh
draw path.

The relevant declarations are in
[`InputLayout`](../source/engine/core/InputLayout.cpp) and the forward shader's
[`VSInput`](../source/engine/renderer/shaders/shaders.hlsl).

### 3.2 Geometry loading

For every scene game object, `Engine::LoadAssets` asks AssetSuite for the same named mesh
three times:

1. `MeshOutputFormat::POSITION`;
2. `MeshOutputFormat::TEXCOORD`;
3. `MeshOutputFormat::NORMAL`.

Color is not imported. `RenderContext::GenerateColors` creates an `RGBA32F` color stream
using a built-in palette.

If any AssetSuite request fails, loading of that game object's mesh stops and no render
item is created for it. Consequently, imported scene meshes currently require positions,
texture coordinates, and normals even when a particular render pass would only read
positions. See [`Engine::LoadAssets`](../source/engine/engine/Engine.cpp).

The engine interprets `MeshDescriptor::numOfVertices` as a triangle count and multiplies it
by three when creating vertex streams and issuing the eventual non-indexed draw. All four
streams must consequently contain matching data for that expanded vertex count.

Normals are normalized on the CPU before their vertex buffer is created. Position data is
also used to calculate the mesh's local axis-aligned bounding box.

### 3.3 Vertex-buffer creation

`RenderContext::CreateVertexBuffer` receives:

```cpp
CreateVertexBuffer(
    UINT numOfVertices,
    UINT numOfFloatsPerVertex,
    FLOAT* meshData,
    const CHAR* name);
```

It performs the following operations:

1. Calculates the resource size as `vertexCount * components * sizeof(float)`.
2. Creates a D3D12 upload-heap buffer.
3. Gives the resource a debug name prefixed with `VB_`.
4. Calculates local bounds from the first three components when at least three components
   are present. Only bounds from the position buffer are later assigned to the mesh.
5. Stores the resource in `RenderContext::vertexBuffers` and returns an `HVertexBuffer`
   containing its vector index.
6. Maps the resource, copies all source data with `memcpy`, and unmaps it.

The `VertexBuffer` wrapper owns the `ID3D12Resource` and releases it in its destructor. The
handle does not own the resource and is only an index into the render context's collection.
See [`VertexBuffer`](../source/engine/core/VertexBuffer.h) and
[`RenderContext::CreateVertexBuffer`](../source/engine/renderer/RenderContext.cpp).

Static geometry remains in upload heaps. It is not staged into default-heap resources.

### 3.4 Mesh construction

`RenderContext::CreateMesh` requires a valid position buffer. Color, texture-coordinate,
and normal buffers are optional and default to `HVertexBuffer::Invalid()`:

```cpp
HMesh mesh = CreateMesh(position, color, texCoord, normal, name);
HMesh positionOnlyMesh = CreateMesh(position);
```

For each valid handle, it indexes `RenderContext::vertexBuffers`, obtains the GPU virtual
address, and constructs a `D3D12_VERTEX_BUFFER_VIEW`. An invalid optional handle produces
an empty view and is retained by `Mesh` to represent an absent stream. The strides are
fixed according to the table in section 3.1. The function returns the handle of the newly
created mesh.

The mesh's vertex count and local bounds are copied from the position buffer. The function
does not currently validate that the other three streams have the same vertex count or
sufficient size.

`Mesh` stores the four buffer handles and their views and exposes `HasPosition`,
`HasColor`, `HasTexture`, and `HasNormals`. It does not own the vertex buffers. See
[`Mesh`](../source/engine/asset/Mesh.h) and
[`RenderContext::CreateMesh`](../source/engine/renderer/RenderContext.cpp).

### 3.5 Input-layout construction

Each graphics pass creates its own `InputLayout`. `AppendElementT` appends semantic and
format declarations in call order. `InputLayout::AppendElement` assigns:

```cpp
element.InputSlot = inputElementsList.size();
element.AlignedByteOffset = 0;
element.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
```

Every element therefore reads offset zero from a different per-vertex buffer. The order in
which streams are appended determines their slots.

The forward pass appends all streams in the order required by the fixed mesh binding:

```cpp
AppendElementT(
    VertexStream::Position,
    VertexStream::Color,
    VertexStream::TexCoord,
    VertexStream::Normal);
```

Its vertex shader declares matching `POSITION`, `COLOR`, `TEXCOORD`, and `NORMAL`
semantics. See [`ForwardPass`](../source/engine/renderer/passes/ForwardPass.cpp).

The shadow-map, selection, highlight-input, and user-interface passes declare only
`Position`. In those layouts, position remains input slot 0.

### 3.6 Binding and drawing

`RenderContext::BindGeometry`:

1. sets `D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST`;
2. binds the required position view to slot 0;
3. conditionally binds present color, texture-coordinate, and normal views to slots 1, 2,
   and 3 respectively.

Streams absent from a mesh are not bound. Streams present on a mesh may still be bound
when the active input layout does not consume them; D3D12 ignores such unused bindings.

`RenderContext::DrawMesh` obtains the mesh's position-derived vertex count and calls:

```cpp
DrawInstanced(vertexCount, 1, 0, 0);
```

Geometry is therefore non-indexed, single-instance, and triangle-list only. There is no
index buffer in the current mesh path. See
[`RenderContext::BindGeometry` and `DrawMesh`](../source/engine/renderer/RenderContext.cpp).

## 4. Required invariants

Correct rendering currently depends on all of the following:

1. A scene mesh provides position, texture-coordinate, and normal data to AssetSuite.
2. Every stream present on a mesh describes the same logical vertices in exactly the same
   order as its position stream.
3. Every present stream has enough entries for the position buffer's vertex count.
4. Every mesh has a valid position handle; other handles may be invalid.
5. Position is bound to slot 0, color to slot 1, texture coordinates to slot 2, and normals
   to slot 3.
6. A pass appending multiple input elements uses the same ordering as the streams bound by
   `BindGeometry`.
7. The vertex shader semantics and formats agree with the pass's input layout.
8. The total vertex count is suitable for a triangle list; normally it is divisible by
   three.

These invariants are mostly implicit. The engine does not validate all of them before
creating or drawing a mesh.

## 5. Missing and unused streams

### 5.1 A pass that only needs position

This is supported for both complete imported meshes and position-only meshes.

The pass declares only `VertexStream::Position`, and its shader declares only a
`POSITION` input. Shadow mapping, selection, highlight input, and user interface use this
pattern.

### 5.2 A mesh that only contains position

This is supported today. Position is mandatory, while color, texture-coordinate, and
normal handles may be invalid. `CreateMesh` creates empty views for absent streams and
`BindGeometry` skips them.

Supplying `nullptr` is not part of the handle API; absence is represented by
`HVertexBuffer::Invalid()`.

`UserInterfacePass` exercises this path with a manually created six-vertex `float4`
position buffer. After composition, it draws a solid-color quad through
`RT_UserInterfacePass`, an RTV that refers to `CompositionTexture`, every frame with no
placeholder color, UV, or normal buffers. Because it does not clear the target, the quad
overlays the composed scene and reaches the normal presentation path.

Imported scene geometry is still less flexible: asset loading abandons a game object when
UV or normal extraction fails, regardless of which passes will render it.

### 5.3 Non-prefix stream combinations

The current input-layout builder assigns slots sequentially rather than assigning stable
slots by semantic. For example:

```cpp
AppendElementT(VertexStream::Position, VertexStream::Normal);
```

would assign normal to slot 1, while `BindGeometry` binds the normal buffer to slot 3.
That combination is invalid under the current implementation. Position-only and the full
four-stream order work because their inferred slots happen to match the fixed binding.

## 6. Known limitations and risks

- Optional streams are limited to the four fields built into `Mesh`; the representation is
  not yet an arbitrary collection of semantic/format/stride declarations.
- `InputLayout` appears generic but does not encode stable semantic-to-slot mappings.
- Tangents can be declared but cannot be supplied by a `Mesh`.
- Stream vertex counts and sizes are not cross-validated.
- Geometry is duplicated per face vertex and drawn without an index buffer.
- Static geometry is read directly from upload heaps rather than default heaps.
- `GenerateColors` allocates a temporary array with `new[]` and does not release it after
  `CreateVertexBuffer` copies the data.
- `CreateVertexBuffer` assumes `meshData` is valid when copying, so it does not support a
  null data pointer despite having fallback bounds logic for one.

## 7. Future requirements: generalized vertex streams

This section is a proposed extension, not a description of current behavior.

Position-only meshes are now supported. The following requirements would generalize and
validate that initial implementation:

1. Each semantic should have a stable input slot independent of append order:
   position 0, color 1, texture coordinate 2, normal 3, tangent 4.
2. Mesh storage should scale beyond the four currently hard-coded stream fields.
3. `CreateMesh` should validate each supplied stream's format, stride, size, and vertex
   count.
4. Binding should explicitly clear stale input slots when required by a pipeline change.
5. Pipeline creation or draw submission should verify that every stream required by the
   active input layout is present on the mesh.
6. Streams present on a mesh but unused by the active pipeline should remain legal.
7. Asset loading should request only the streams required by the intended mesh/material
   path, or generate documented defaults where appropriate.

The current creation interface already uses invalid handles for optional fields. A future
descriptor may become useful when topology, indices, arbitrary streams, and validation
policies are added; it is not needed solely to represent an absent handle.

## 8. Acceptance scenarios for a future implementation

These scenarios can become tests for the current position-only path and its future
validation.

### Position-only volume

- Given a mesh containing only a valid position stream,
- and a pipeline whose input layout contains only `POSITION` at slot 0,
- drawing succeeds without placeholder buffers or debug-layer errors.

### Full forward mesh

- Given position, color, texture-coordinate, and normal streams with equal vertex counts,
- and the current forward input layout,
- rendering remains equivalent to the current output.

### Missing required stream

- Given a mesh without normals,
- and a pipeline requiring `NORMAL`,
- the engine rejects the draw with a clear diagnostic before submitting invalid geometry.

### Unused available stream

- Given a complete four-stream mesh,
- and a position-only pipeline,
- the draw succeeds and only the required position stream needs to be bound.

### Mismatched stream lengths

- Given position and UV streams with different vertex counts,
- mesh creation fails with a diagnostic identifying the mismatched streams.

## 9. Implementation map

| Responsibility | Current location |
|---|---|
| Asset stream extraction and mesh assembly | [`Engine.cpp`](../source/engine/engine/Engine.cpp) |
| Vertex-buffer allocation and upload | [`RenderContext.cpp`](../source/engine/renderer/RenderContext.cpp) |
| Vertex-buffer ownership and metadata | [`VertexBuffer.h`](../source/engine/core/VertexBuffer.h) |
| Mesh views, count, and bounds | [`Mesh.h`](../source/engine/asset/Mesh.h) |
| Handle representation | [`Handle.h`](../source/engine/asset/Handle.h) |
| Semantic, format, and inferred input-slot construction | [`InputLayout.cpp`](../source/engine/core/InputLayout.cpp) |
| Full stream declaration | [`ForwardPass.cpp`](../source/engine/renderer/passes/ForwardPass.cpp) |
| Example position-only pipeline | [`ShadowMapPass.cpp`](../source/engine/renderer/passes/ShadowMapPass.cpp) |
| Position-only mesh creation and draw | [`UserInterfacePass.cpp`](../source/engine/renderer/passes/UserInterfacePass.cpp) |
| Full vertex shader input contract | [`shaders.hlsl`](../source/engine/renderer/shaders/shaders.hlsl) |
