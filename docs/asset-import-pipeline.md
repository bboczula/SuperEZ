# Asset Import Pipeline — Requirements

Goal: automate turning an arbitrary downloaded model (e.g. `bmw.zip`) into a loadable
SuperEZ scene under `bin/Editor/assets/<sceneName>/`, replacing the manual
Blender-export-and-hand-edit workflow.

## 1. What the engine actually requires (derived from code)

### 1.1 Scene folder layout

`Engine::LoadSceneAssets` (source/engine/engine/Engine.cpp:529) resolves everything from
the working directory:

```
assets/<sceneName>/<sceneName>.xml      <- scene descriptor (name MUST match folder)
assets/<sceneName>/<library>.obj        <- single mesh library for the whole scene
assets/<sceneName>/<texture files>      <- referenced by filename, relative to scene folder
```

The scene descriptor is **XML, not JSON** (tinyxml2). The startup scene name is currently
hardcoded in `GetStartupSceneName()` (source/game/main.cpp:132, source/sceneviewer/main.cpp:30).

### 1.2 Scene XML contract (`Engine::ProcessScene` and friends, Engine.cpp:338–477)

- Root `<Scene>` element — required.
- `<MeshLibrary file="X.obj"/>` — required (hard assert), exactly one, filename relative
  to the scene folder. All meshes for the scene come from this single OBJ.
- At least one `<Camera name="...">` with **required** `<Position x y z/>` and
  `<Rotation pitch yaw roll/>` children (hard asserts). Optional: `active`, `type="orthographic"`,
  `<Projection width height/>`.
- Zero or more `<Sunlight>` (optional; Direction/Color/Lighting children).
- One `<GameObject name="..." mesh="..." texture="...">` per visible object, each with
  **required** `<Position/>`, `<Rotation/>`, `<Scale/>` children (hard asserts).
  - `mesh` = group name inside the library OBJ.
  - `texture` = image filename relative to the scene folder. There is **no untextured
    path** — every GameObject must reference a texture file that exists.

### 1.3 OBJ mesh library contract (AssetSuite WAVEFRONT decoder + Engine::LoadAssets)

The engine asks AssetSuite for POSITION, TEXCOORD and NORMAL streams per mesh
(Engine.cpp:278–305) and **silently skips the object if any stream is missing**. So every
group must have:

- `g <MeshName>` per object (existing assets use Blender's "Object Groups" export:
  `g Earth_Mesh`, `g Black_Pawn_08_Mesh`, ... — `_Mesh` suffix is convention, not enforced).
- Fully **triangulated** faces (`f a/b/c` triples only — all existing assets are triangles).
- **Positive, absolute 1-based indices**. (Blender emits these; the bmw.obj from 3ds Max
  uses *negative/relative* indices — must be rewritten.)
- `v`, `vt` and `vn` data for every face vertex (`f v/vt/vn` form).
- Axis convention matching the existing assets: Blender OBJ export defaults
  (**-Z forward, Y up**), which is what milkyway/chess/sponza were exported with.
  Renderer is D3D12, Y-up.

`usemtl`/`mtllib`/`s` lines are present in existing assets and tolerated, but materials
are **not** used by the engine — texture binding comes solely from the scene XML.

### 1.4 Texture contract

- AssetSuite decodes BMP, PNG and PPM (`ImageDecoders::Auto` by extension), output
  converted to RGB8 and uploaded as RGBA8 with a full mip chain.
- In practice **every shipping scene references 24-bit `.bmp`** files; the chess PNGs were
  hand-converted to BMP. Target format for the pipeline: **24-bit uncompressed BMP**.
- One texture per GameObject (`TEX_<meshName>` created per object; no sharing at the GPU
  level even when the file is shared).

## 2. What the sample input looks like (bmw.zip)

| Property | bmw.obj | Engine expectation | Action needed |
|---|---|---|---|
| Exporter | 3ds Max guruware 2007 | Blender-style | rewrite |
| Objects | 54 (`o name` + `g name` pairs) | one `g` per object | re-emit groups |
| Faces | all triangles, `v/vt/vn` | triangles | none |
| Indices | **negative (relative)** | positive absolute | convert |
| UVs / normals | present | required | none |
| Materials | 30+ color-only (`Kd`, no `map_Kd`) | texture file per object | **bake solid-color BMPs** |
| Scale | coordinates in the hundreds | existing scenes ~1 unit | rescale option |
| Axes | 3ds Max export (unverified up-axis) | Y-up, Blender-convention | axis transform option |

## 3. Tool requirements

A command-line importer (proposed: single Python script, `tools/import_scene.py`, stdlib
only — no Blender or pip dependency):

```
python tools/import_scene.py <input.zip|input.obj> --name bmw [options]
```

### 3.1 Functional

1. **Input handling**: accept a `.zip` (extract to temp) or a bare `.obj`; locate the OBJ
   and its `.mtl` and texture files.
2. **OBJ normalization** — parse and re-emit the OBJ as a clean mesh library:
   - Convert negative/relative indices to absolute 1-based.
   - Triangulate quads/n-gons (fan triangulation).
   - Emit one `g <SanitizedName>_Mesh` per source object (`o` or `g`), deduplicate names,
     sanitize to XML-attribute-safe ASCII.
   - Generate flat normals for faces missing `vn`; generate dummy UVs (e.g. planar or
     all-zero) for faces missing `vt`, so the engine never silently skips a mesh.
   - Optional `--scale <f>` / `--fit <size>` uniform rescale (auto-fit scene bounds to a
     target size, e.g. longest axis = 10 units).
   - Optional axis conversion `--up z` etc. (swap/negate axes) for non-Blender sources.
   - Drop unused data (per-object re-indexing so vertex data stays compact).
3. **Texture generation** — one BMP per referenced material:
   - `map_Kd` present → convert image to 24-bit BMP (PNG/JPG/TGA input at minimum PNG+BMP;
     stdlib can do BMP natively, PNG via `zlib`).
   - Color-only material → bake an N×N (e.g. 8×8) solid-color BMP from `Kd`
     (e.g. `mat_CarShellNew.bmp`).
   - No material → fall back to a generated `missing.bmp` checker.
4. **Scene XML generation** — `<sceneName>.xml` with:
   - One default `<Camera>` positioned automatically from the scene bounding box (looking
     at center, distance ~1.5× bounds diagonal), so the scene is visible on first load.
   - One default `<Sunlight>`.
   - One `<GameObject>` per mesh group: `name` = object name, `mesh` = group name,
     `texture` = the material's baked/converted BMP, identity Position/Rotation/Scale.
5. **Output**: write everything to `bin/Editor/assets/<sceneName>/`; refuse to overwrite
   an existing scene folder unless `--force`.
6. **Validation report**: after writing, re-parse the output and print a summary — mesh
   count, triangle count, texture count, warnings (skipped/empty objects, missing UVs
   that were generated, name collisions) — and fail loudly rather than producing a scene
   the engine would assert on.

### 3.2 Non-functional

- Must handle a 31 MB / ~1M-line OBJ (bmw) in seconds, streaming line-by-line.
- Deterministic output (stable ordering) so re-imports diff cleanly in git.
- Windows-friendly paths; no external tools required.

## 4. Decisions (2026-07-02)

1. **Axes**: default is pass-through (assume source is already Y-up like the engine);
   `--up z` / `--up -z` flags convert Z-up sources. To be verified visually per import.
2. **BMP**: the tool always emits 24-bit BMP (converts PNG input, copies BMP input).
3. **Scale**: preserved by default; `--scale <f>` for manual override. The tool warns and
   suggests a factor when the scene is too large for the engine's fixed farPlane (1000).
4. **Solid-color textures**: baked as 8×8 BMPs from the material `Kd` (gamma-encoded),
   named `mat_<material>.bmp`.
5. **Tool**: plain Python 3, stdlib only — `tools/import_scene.py`. A headless-Blender
   preprocessing pass can be added later for formats OBJ can't express (FBX, glTF).
6. **`_Mesh` suffix**: appended to group names (stripped first if already present).
7. **Multi-material objects**: split into one group/GameObject per material, since the
   engine binds exactly one texture per GameObject.
