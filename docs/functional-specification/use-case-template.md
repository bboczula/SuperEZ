# FS-DOM-000 - Use Case Name

Status: Draft
Owner: Unassigned
Last reviewed: YYYY-MM-DD

## 1. Characteristic information

| Field | Specification |
|---|---|
| ID | FS-DOM-000 |
| Name | Short active verb phrase |
| Mapped operation | Exact owner and operation name for a supporting-function use case, or Not applicable for a higher-level use case |
| Goal | Result needed by the primary actor |
| Scope | System whose behavior is being specified |
| Level | Summary, user goal, or supporting function |
| Primary actor | Existing SuperEZ component, person, device, or external system that initiates the use case |
| Trigger | Event that starts the use case |
| Frequency | Expected rate or lifecycle occurrence |
| Priority | Critical, high, medium, or low |

## 2. Stakeholders and interests

| Stakeholder | Interest |
|---|---|
| Role or system | Outcome or protection needed by that stakeholder |

## 3. Systems involved

| System or role | Responsibility in this use case |
|---|---|
| Existing initiating component | Initiates the interaction and supplies the required information. |
| Existing component under specification | Coordinates the behavior and delivers the result. |
| Existing supporting component | Supplies a service required by the interaction. |

Use repository component names exactly. If a needed responsibility has no current owner,
name the new component and label it Proposed.

The mapped operation is an anchor, not scenario prose. Do not include parameters,
signatures, statements, algorithms, or source locations in the behavioral sections.

## 4. Conditions and guarantees

### Preconditions

1. State a fact guaranteed before the interaction begins.

### Minimal guarantees

1. State what remains true after every exit, including failure.

### Success guarantees

1. State the observable result after successful completion.

## 5. Main success scenario

| Step | From | To | Interaction and required result |
|---:|---|---|---|
| 1 | Primary actor | System under specification | Describe the request in natural language. |
| 2 | System under specification | Supporting actor | Describe the required cooperation and outcome. |
| 3 | System under specification | Primary actor | Describe how the actor's goal is delivered. |

## 6. Alternative flows

| ID | Branch point | Condition | Flow and rejoin point |
|---|---:|---|---|
| A1 | Step 2 | Describe a valid variation. | Describe the alternative behavior and the step at which the normal flow resumes. |

## 7. Exception flows

| ID | Failure point | Condition | Required response | End state |
|---|---:|---|---|---|
| E1 | Step 2 | Describe a detectable failure. | Describe the rejection, diagnostic, cleanup, or recovery. | State whether the use case fails or recovers. |

## 8. Special requirements

| ID | Category | Requirement |
|---|---|---|
| SR1 | Performance | State a measurable constraint. |
| SR2 | Diagnostics | State the information required to identify a failure. |

## 9. Use-case relationships

| Relationship | Use case | Interaction point |
|---|---|---|
| Includes, extends, specializes, precedes, follows, or see also | Replace this text with a real use-case link. | State the step or reason for the relationship. |

Write None when no relationship exists. Do not leave an unresolved placeholder link.
When a higher-level use case invokes a mapped supporting function as a required part of its
normal flow, use Includes and link its stable use-case identifier.

## 10. Acceptance scenarios

### Main success

- Given the stated preconditions,
- when the initiating event occurs with valid information,
- then the success guarantees are observable.

### Exception E1

- Given the stated preconditions,
- when the named failure condition occurs,
- then the required response and minimal guarantees are observable.

## 11. Open issues

| ID | Question | Owner | Decision or status |
|---|---|---|---|
| OI-DOM-001 | State the decision that is needed. | Responsible role | Open |

## 12. Change history

| Date | Change | Author |
|---|---|---|
| YYYY-MM-DD | Initial draft | Name or role |
