# SuperEZ Functional Specification

Status: living specification
Specification owner: Requirements Engineer or System Analyst
Last reviewed: 2026-08-30

This specification defines what SuperEZ must do as a collection of detailed use cases.
Each supporting-function use case may be anchored to one existing SuperEZ operation, but
its behavior is still written in natural language. The operation name provides stable
traceability and a link target for higher-level use cases; it does not turn the scenario
into a source-code walkthrough.

The format is based on the fully dressed use-case style commonly used in systems and
requirements engineering. Each use case identifies the participating roles and systems,
the conditions before and after the interaction, the normal sequence, variations,
exceptions, constraints, relationships, and acceptance scenarios.

## 1. Specification roles

| Role | Responsibility |
|---|---|
| Specification Owner | Discovers the feature, defines its scope and required behavior, maintains use-case relationships, resolves open questions, and approves specification changes. |
| System Architect | Confirms system boundaries, responsibilities, interfaces, quality constraints, and architectural feasibility. |
| Developer | Uses approved use cases to prepare a technical design, implementation plan, and development tasks. |
| Test Engineer | Derives normal, alternative, exception, boundary, and regression tests from the specified scenarios. |
| Reviewer or Stakeholder | Confirms that the specified outcomes solve the intended engine or tool need. |

The corresponding role in a telecommunications organization may be called System
Specification Engineer, Requirements Engineer, System Analyst, Feature Specification
Owner, or System Architect. The title is less important than the responsibility: this role
owns the functional contract before developers decide how to implement it.

## 2. Use-case catalog

| ID | Use case | Mapped operation | Domain | Level | Status | Primary actor |
|---|---|---|---|---|---|---|
| [FS-TEX-001](FS-TEX-001-create-texture.md) | Create Texture | `RenderContext::CreateTexture` | Textures | Supporting function | Proposed | Engine |
| [FS-GEO-001](FS-GEO-001-create-vertex-buffer.md) | Create Vertex Buffer | `RenderContext::CreateVertexBuffer` | Geometry | Supporting function | Proposed | Engine |

New cases should be created from the [use-case template](use-case-template.md). Keeping one
use case per Markdown file makes the set easy to review, evolve, and transfer to a GitHub
Wiki.

## 3. Identifier and relationship rules

Every use case receives a stable identifier containing a domain and sequence number. The
initial domain names are:

| Domain | Meaning |
|---|---|
| TEX | Texture creation, preparation, sampling, and lifetime |
| GEO | Geometry creation, preparation, binding, and drawing |
| REN | Rendering stages and frame execution |
| AST | Asset loading, conversion, and scene import |
| EDT | Editor behavior |
| SYS | Behavior spanning several engine areas |

Use-case links state a precise relationship:

| Relationship | Meaning |
|---|---|
| Includes | The base use case always performs the referenced reusable use case. |
| Extends | Optional or conditional behavior is attached to a defined point in the referenced use case. |
| Specializes | The use case is a more specific form of another use case. |
| Precedes or Follows | The use cases have an ordering dependency, but neither contains the other. |
| See also | The use cases are relevant to one another without a behavioral dependency. |

Relationships describe required behavior between use cases. They must not be inferred from
the internal organization of the software.

## 4. Writing rules

1. Give every use case a stable identifier and an active verb phrase as its name.
2. For a supporting-function use case, record exactly one mapped operation in the
   characteristic-information table. This is the only place where the operation's
   programming-language identifier is required.
3. Use the exact name of an existing SuperEZ component when that component owns the
   interaction, such as Engine, Render Context, Device Context, Window Context, Descriptor
   Heap, Command List, or a named render pass.
4. Introduce a new component name only when no existing component owns the required
   responsibility. Mark such a component as proposed so that readers do not mistake it for
   part of the current architecture.
5. Describe what participants communicate and what outcomes they require, not how the
   software performs the work internally.
6. Keep the main success scenario unconditional. Put decisions in alternative or exception
   flows and attach them to a numbered main-flow step.
7. Write each step as a natural-language interaction between a sender and receiver.
8. State preconditions as facts guaranteed before entry. Do not repeat them as validation
   steps unless validation itself is an externally relevant requirement.
9. State minimal guarantees that hold on every exit and success guarantees that hold only
   after successful completion.
10. Record measurable quality requirements separately from the functional sequence.
11. Give every open decision a stable identifier and record its resolution rather than
   silently rewriting history.
12. Derive at least one acceptance scenario from the normal flow and every required
    exception flow.
13. A higher-level use case refers to a supporting-function use case by relationship and
    identifier. For example, Load Assets may state that it includes
    [FS-TEX-001 - Create Texture](FS-TEX-001-create-texture.md).

The following content does not belong in this specification:

- programming-language names or signatures outside the single mapped-operation field;
- pseudocode or algorithms;
- class, function, variable, or container names;
- source-file links or line references;
- memory-layout details unless they are an external interface requirement;
- a chosen implementation where several implementations could satisfy the same behavior.

Source-level details beyond the mapped operation belong in a separate technical design
linked from the use case after the functional behavior has been approved.

## 5. Lifecycle

| State | Meaning |
|---|---|
| Draft | Behavior is being discovered and may change freely. |
| Proposed | Ready for review by architecture, development, and testing. |
| Approved | Accepted as the implementation target. |
| Verified | The delivered system has been tested against the approved acceptance scenarios. |
| Superseded | Replaced by another identified specification. |

## 6. Planning from the specification

For an approved use case, development planning normally produces:

1. a technical design identifying the chosen components and interfaces;
2. implementation tasks for the normal flow and independently deliverable slices;
3. tasks for alternative and exception handling;
4. tests mapped to scenario and exception identifiers;
5. follow-up work for accepted limitations and unresolved issues.

The technical design may change without changing this specification as long as the
observable interactions, outcomes, guarantees, and quality requirements remain satisfied.

## 7. Method references

- [Alistair Cockburn's use-case template](https://www.cs.otago.ac.nz/coursework/cosc461/uctempla.htm)
  presents the characteristic information, main success scenario, extensions, variations,
  and related-use-case fields behind this format.
- [ISO/IEC/IEEE 29148](https://www.iso.org/standard/72089.html) provides the broader
  requirements-engineering framework. It does not prescribe this exact Markdown layout.
