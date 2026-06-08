# SuperEZ C++ Code Guidelines

These guidelines describe the style already used in SuperEZ and turn it into a consistent target for new engine, renderer, game, and test code.

## Project Defaults

- Use C++20 for engine, game, and scene viewer code.
- Keep the project Windows-first and DirectX 12-first.
- Use CRLF line endings and keep a final newline, matching `.editorconfig`.
- Treat warnings seriously. Code should be clean under the existing Premake warning settings.
- Prefer simple, explicit engine code over clever generic abstractions.
- Do not refactor vendored code under `source/externals` unless the dependency itself is being updated.

## File Layout

- Pair most concrete engine types as `Name.h` plus `Name.cpp`.
- Keep small template-only ECS helpers in headers.
- Use `#pragma once` in headers.
- Put local project includes before standard-library includes when the file already follows that shape.
- Keep includes direct enough that each file can stand on its own. Forward declare engine classes in headers when only pointers or references are needed.

Typical header shape:

```cpp
#pragma once

#include "../asset/Handle.h"

#include <vector>

class DeviceContext;

class Example
{
public:
	void Initialize(DeviceContext* deviceContext);
private:
	HTexture texture;
};
```

## Naming

- Use `PascalCase` for classes, structs, enums, and public member functions.
- Use `camelCase` for local variables, parameters, and most data members.
- Use `m_` only where the surrounding subsystem already uses it, such as ECS internals.
- Use short handle aliases with an `H` prefix: `HTexture`, `HBuffer`, `HMesh`, `HRenderTarget`.
- Use `I` prefixes for interfaces: `IGame`, `IInput`, `IScene`, `IEngineState`.
- Use `*Desc`, `*Data`, `*Constants`, and `*Component` suffixes for plain data structs.
- Use `Create*`, `Get*`, `Set*`, `Bind*`, `Transition*`, `Clear*`, and `Draw*` verbs consistently for render-context operations.
- Spell API names carefully. Existing typos like `CreateConsantBuffer`, `CleraRenderTarget`, and `wasObjectSeleced` should not be copied into new APIs.

## Types And Data

- Prefer `enum class` for new enums, with an explicit small underlying type when useful.
- Use plain structs with default member initializers for scene data, components, constants, and descriptor objects.
- Use descriptor structs for functions with many creation options, following `TextureCreateDesc`.
- Prefer `UINT`, `BYTE`, `BOOL`, `LPCWSTR`, and other Win32/D3D types at D3D-facing boundaries.
- Prefer standard types such as `std::size_t`, `std::uint32_t`, `bool`, and `std::string` in engine/game-facing APIs.
- Use named invalid sentinels instead of repeating magic values. Prefer `Handle<T>::Invalid()`, `InvalidEntity`, or a local `static constexpr` over raw `0xffffffff` or `~0u`.

## Ownership

- Renderer resource pools currently store owning raw pointers and return typed handles. If adding to those pools, keep the pattern consistent:
  - allocate in the owning context,
  - store in the matching vector,
  - return an `H*` handle,
  - release/delete during unload or destruction.
- Use `SafeRelease` for raw COM interface pointers managed by wrapper classes.
- Use `Microsoft::WRL::ComPtr` for temporary COM blobs or short-lived COM values when the surrounding code already does.
- Prefer `std::unique_ptr` for new non-D3D service ownership when it does not fight an existing raw-pointer lifecycle.
- Do not make callers own renderer resources directly. Route creation through `RenderContext` or `DeviceContext`.
- If a function receives a pointer that may be null, name and handle that condition explicitly. If it must not be null, assert or fail early.

## Error Handling

- Use `ExitIfFailed` or `DX_TRY` for HRESULT-returning D3D/Win32 work.
- Use `assert` for internal invariants and invalid engine states.
- Avoid exceptions in engine code.
- Prefer early returns for recoverable runtime conditions, especially service methods like `TryGet*` and scene queries.
- Log useful debug breadcrumbs with `OutputDebugString` around important D3D lifecycle steps.

## Rendering Code

- Put D3D object creation in `DeviceContext`, `RenderContext`, or the specific resource wrapper.
- Keep render-pass classes small and lifecycle-driven:
  - constructor names the pass and shader,
  - `ConfigurePipelineState` builds layouts, root signatures, render targets, and buffers,
  - `Initialize` performs pass-specific setup,
  - `Update` handles per-frame state,
  - `Execute` records rendering commands,
  - `PostSubmit` handles after-submit work.
- Use `RootSignatureBuilder` for new root signatures.
- Keep resource state changes explicit with `TransitionTo` and `TransitionBack`.
- Bind through `RenderContext`; do not reach into descriptor heaps or command lists from game code.
- Name GPU resources and debug objects. D3D debug names are part of the development experience.
- Use constants for root slots, descriptor slots, viewport assumptions, and invalid IDs when a value appears more than once.

## ECS And Services

- Keep components as small data-only structs.
- Register every component before adding it to entities.
- Use `Coordinator` for entity and component operations instead of talking to managers from game code.
- Keep game-facing behavior behind service interfaces such as `IScene`, `IInput`, and render services.
- For scene operations, prefer `FindEntityByName`, `GetPosition`, `SetPosition`, `SetActiveCamera`, and similar service APIs over direct component access.
- Return `InvalidEntity` for failed entity lookups.

## Game Code

- Implement games by deriving a `final` app class from `IGame`.
- Keep the game loop split across `OnInit`, `OnUpdate`, and `OnShutdown`.
- Cache entity IDs and initial state in `OnInit` when the same entities are used every frame.
- Use `FrameTime` for time-dependent behavior.
- Keep console logging acceptable in game/sample code, but prefer engine debug logging inside engine systems.

## Functions

- Keep functions focused on one engine operation.
- Use `const` for accessors and read-only APIs.
- Pass heavy objects by `const&`.
- Use `auto` for verbose iterator/resource types when the initializer makes the type obvious.
- Use explicit types for IDs, handles, counts, and API boundaries where readability matters.
- Use lambdas for local helper steps inside a larger operation, especially asset loading or descriptor setup.
- Keep helper lambdas close to where they are used.

## Formatting

- Use tabs for the dominant engine indentation style.
- Put opening braces on the next line for functions, classes, structs, loops, and conditionals.
- Keep one blank line between functions.
- Prefer this constructor initializer style:

```cpp
Buffer::Buffer(...)
	: resource(resource)
	, layout(layout)
	, currentState(initialState)
{
}
```

- Keep public methods before private data in classes.
- Put simple one-line accessors inline in headers when they are genuinely trivial.

## Comments

- Use comments to explain engine intent, D3D constraints, or temporary hacks.
- Avoid comments that only restate the line below them.
- Mark known temporary work honestly, but include enough context that the next pass knows what to fix.
- Prefer precise comments like "Root Constants @ b0" or "CBV b3 (light view-projection)" around binding layout code.

## Tests

- Use GoogleTest for unit tests.
- Keep tests close to the subsystem being validated under `source/tests`.
- Test math, camera behavior, ECS behavior, and pure service logic before renderer behavior.
- Avoid tests that require a full D3D device unless the test is specifically an integration/render test.

## New Code Checklist

- Does the file belong under `engine`, `game`, `sceneviewer`, `tests`, or `externals`?
- Is ownership clear at the call site and at destruction/unload time?
- Are invalid handles/entities represented with named sentinels?
- Are D3D calls checked?
- Are root slots and descriptor slots named or clearly commented?
- Can game code reach this only through the right service/API boundary?
- Does the naming match nearby code?
- Did you avoid copying existing typos into new API names?
