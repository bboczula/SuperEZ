# FS-GEO-001 - Create Vertex Buffer

Status: Proposed
Owner: Specification Owner
Last reviewed: 2026-08-30

## 1. Characteristic information

| Field | Specification |
|---|---|
| ID | FS-GEO-001 |
| Name | Create Vertex Buffer |
| Mapped operation | `RenderContext::CreateVertexBuffer` |
| Goal | Make one complete stream of vertex information available to rendering and return a stable reference to it. |
| Scope | Render Context |
| Level | Supporting function |
| Primary actor | Engine |
| Trigger | Engine asks Render Context to create a vertex buffer for imported geometry. |
| Frequency | Once for every vertex stream required by imported geometry; additionally when a render pass creates procedural geometry |
| Priority | Critical |

## 2. Stakeholders and interests

| Stakeholder | Interest |
|---|---|
| Engine | Receives a stable reference to a complete copy of an imported vertex stream. |
| User Interface Pass | Can create procedural geometry through the same Render Context behavior. |
| Render Context | Owns the completed vertex buffer and accurate descriptive information. |
| Device Context | Receives a valid request for graphics storage. |
| Mesh | Can use the completed buffer with the correct vertex count, layout, role, and spatial bounds. |
| Rendering passes | Read the expected information for every vertex without missing or excess data. |

## 3. Systems involved

| System or component | Responsibility in this use case |
|---|---|
| Engine | Supplies an imported vertex stream, vertex count, values per vertex, semantic role, name, and lifetime. |
| User Interface Pass | May initiate the same behavior for procedural user-interface geometry. |
| Render Context | Validates the request, calculates spatial bounds when appropriate, coordinates graphics storage, copies the stream, records ownership, and returns the reference. |
| Device Context | Creates storage that is accessible to both Render Context during creation and rendering passes during drawing. |
| Mesh | Later associates the completed buffer with the appropriate vertex role. It does not participate in creation. |

Descriptor Heap, Command List, and Window Context do not participate in this use case. The
current behavior creates directly writable graphics storage and completes the copy before
returning the reference.

## 4. Conditions and guarantees

### Preconditions

1. Render Context and Device Context are initialized.
2. Engine supplies a positive vertex count and a supported number of values per vertex.
3. The supplied stream contains exactly the amount of information required by the declared
   count and layout.
4. The declared semantic role agrees with the stream layout.
5. Sufficient graphics storage and Render Context capacity are available.

### Minimal guarantees

1. An unsuccessful request does not return a usable vertex-buffer reference.
2. An unsuccessful request does not expose a partially copied stream to Mesh or rendering
   passes.
3. Storage reserved for a failed request is released when safe.
4. The initiating component receives a diagnostic identifying the buffer and failure
   reason.
5. Previously created geometry remains valid and unchanged.

### Success guarantees

1. Graphics storage contains a complete and exact copy of the supplied vertex stream.
2. Render Context records the vertex count, layout, semantic role, name, lifetime, and any
   applicable spatial bounds.
3. Mesh can later associate the buffer with its declared vertex role.
4. The initiating component receives one stable vertex-buffer reference.
5. The initiating component may release or reuse its original source information after
   completion is confirmed.

## 5. Main success scenario

| Step | From | To | Interaction and required result |
|---:|---|---|---|
| 1 | Engine | Render Context | Request vertex-buffer creation and provide the vertex stream, vertex count, layout, semantic role, name, and lifetime. |
| 2 | Render Context | Engine | Confirm that the request is complete, internally consistent, and supported. |
| 3 | Render Context | Render Context | Determine the graphics-storage requirement from the declared count and layout. |
| 4 | Render Context | Device Context | Request storage large enough for the complete vertex stream and suitable for direct creation-time copying. |
| 5 | Device Context | Render Context | Reserve the requested storage and confirm that it is ready. |
| 6 | Render Context | Render Context | When the stream represents positions, calculate its minimum and maximum spatial extent. |
| 7 | Render Context | Device Context | Copy the complete vertex stream into the reserved storage. |
| 8 | Device Context | Render Context | Confirm that the copied stream is available for rendering reads. |
| 9 | Render Context | Render Context | Record the buffer's ownership, lifetime, name, vertex count, layout, semantic role, and applicable bounds. |
| 10 | Render Context | Engine | Return the stable vertex-buffer reference and confirm successful creation. |

## 6. Alternative flows

| ID | Branch point | Condition | Flow and rejoin point |
|---|---:|---|---|
| A1 | Step 1 | User Interface Pass requests procedural geometry instead of Engine requesting imported geometry. | User Interface Pass supplies the stream description and becomes the recipient in steps 2 and 10. The remaining flow is unchanged. |
| A2 | Step 6 | The stream represents color, texture coordinates, normals, or another non-position role. | Render Context skips spatial-bounds calculation and resumes at step 7. |
| A3 | Step 1 | A compatible immutable vertex stream already exists. | Render Context records shared use and returns the existing stable reference. The use case ends without requesting duplicate storage from Device Context. |
| A4 | Step 1 | The stream is expected to change after creation. | Render Context applies the approved update policy before requesting suitable storage from Device Context. The flow resumes at step 3. |

## 7. Exception flows

| ID | Failure point | Condition | Required response | End state |
|---|---:|---|---|---|
| E1 | Step 2 | The vertex count, layout, role, name, or supplied amount of information is missing, inconsistent, or unsupported. | Render Context rejects the request and reports every detected validation problem to the initiating component. | Failed; no vertex buffer is published. |
| E2 | Steps 3-5 | The required storage size cannot be represented safely or Device Context cannot reserve sufficient storage. | Render Context rejects the request, releases any reservation, and reports the size or resource failure. | Failed; existing geometry is unchanged. |
| E3 | Step 6 | A position stream contains invalid spatial values. | Render Context rejects the position stream, releases reserved storage, and identifies the invalid information. | Failed; no vertex buffer is published. |
| E4 | Steps 7-8 | The copy fails or Device Context cannot confirm completion. | Render Context keeps the buffer unavailable, releases storage when safe, and reports the failed stage. | Failed; Mesh and rendering passes cannot observe incomplete data. |
| E5 | Step 9 | Render Context cannot record ownership or descriptive information. | Render Context withholds the reference, reverses creation when safe, and reports registration failure. | Failed; no unowned vertex buffer remains visible. |

## 8. Special requirements

| ID | Category | Requirement |
|---|---|---|
| SR1 | Completeness | The entire declared stream shall be copied before Render Context returns its reference. |
| SR2 | Fidelity | The completed stream shall preserve the order and values supplied by Engine or User Interface Pass. |
| SR3 | Bounds | Render Context shall calculate spatial bounds only for streams explicitly identified as positions. |
| SR4 | Diagnostics | Every failure report shall identify the buffer name, initiating component, failed step, and reason. |
| SR5 | Resource lifetime | Device Context storage and Render Context ownership shall respect the declared lifetime and update policy. |
| SR6 | Identity | The returned reference shall identify only a vertex buffer and keep the same meaning in Engine, Render Context, Mesh, and rendering passes. |
| SR7 | Scale | Render Context shall reject requests whose declared size cannot be represented or stored safely. |

## 9. Use-case relationships

| Relationship | Use case | Interaction point |
|---|---|---|
| See also | [FS-TEX-001 - Create Texture](FS-TEX-001-create-texture.md) | Both use cases ask Render Context and Device Context to create rendering resources, but neither requires the other. |

This use case is intended to be linked with an Includes relationship from a future Load
Assets use case at every step where Engine creates a required geometry stream.

Related domain description: [Handling Geometry](../handling-geometry.md).

## 10. Acceptance scenarios

### Main success - imported position stream

- Given a valid imported position stream and sufficient graphics storage,
- when Engine asks Render Context to create its vertex buffer,
- then Device Context provides the required storage, Render Context records the spatial
  bounds, and Engine receives a stable reference to the complete stream.

### Alternative A1 - procedural user-interface geometry

- Given a valid procedural position stream,
- when User Interface Pass asks Render Context to create its vertex buffer,
- then the same guarantees apply and User Interface Pass receives the stable reference.

### Alternative A2 - non-position stream

- Given a valid texture-coordinate or color stream,
- when Engine asks Render Context to create its vertex buffer,
- then the complete stream becomes available without spatial-bounds calculation.

### Exception E2 - insufficient storage

- Given a valid stream but insufficient graphics storage,
- when Device Context rejects the storage request,
- then Render Context returns no reference, preserves existing geometry, and reports the
  resource failure to Engine.

## 11. Open issues

| ID | Question | Owner | Decision or status |
|---|---|---|---|
| OI-GEO-001 | Which vertex roles and layouts must Render Context support initially? | System Architect | Open |
| OI-GEO-002 | Which update policies must Device Context support for static, dynamic, and temporary geometry? | Renderer Owner | Open |
| OI-GEO-003 | What information uniquely identifies an immutable stream that Render Context may reuse? | Specification Owner | Open |
| OI-GEO-004 | Should directly writable graphics storage remain part of the requirement, or may a future technical design use a separate transfer stage? | System Architect | Open |

## 12. Change history

| Date | Change | Author |
|---|---|---|
| 2026-08-30 | Initial proposed specification | Codex |
| 2026-08-30 | Rewritten as a code-agnostic functional contract | Codex |
| 2026-08-30 | Replaced generic participants with SuperEZ architectural components | Codex |
| 2026-08-30 | Anchored the use case to its Render Context operation | Codex |
