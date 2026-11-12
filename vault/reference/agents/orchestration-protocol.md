---
id: orchestration-protocol
description: "Planning procedure of the Planner for every request: resume, restate, GDD check, plan, confirm. Execution is in execution-protocol. Open to plan or change planning."
parent: agents-index
kind: rule
applies_to: orchestrator
---
# Orchestration protocol: planning

`/task` splits the procedure between two agents (owner 2026-10-26). The [[agent-planner]] does steps 0 to 4 and writes every decision into the task nodes. The [[agent-executor]] does steps 5 and 6 ([[execution-protocol]]) from those nodes only, in a fresh context, and does not recall the Planner.

The Planner decides WHAT and how it is judged; the Executor decides HOW (owner 2026-11-05).

The relay routes each request to a family first ([[request-families]]): réglage skips this protocol; comportement, visuel and gros morceau come here, and the Planner writes the family on each global (`family` in [[task-spec]]).

## 0. Resume first
Read [[tasks-to-do-index]]. If a global task is `in-progress`, report where it stands before taking a new request.

## 1. Restate
Write the request as a goal with observable acceptance checks. If a sentence can be read two ways with opposite effects, ask before planning.

One logically separate point of the request = one global task (task-NNN). A play-test message with ten points gives up to ten global tasks, never one task with ten parts (owner 2026-10-06).

## 2. GDD check
Descend to the gdd nodes the request touches. Report to the owner, before any work:
- conflicts between the request and the GDD;
- values the work needs that are marked `open`;
- scope outside the current milestone.
If any exist, stop and ask whether to continue, adjust the request, or update the GDD.

Do not explore code, tests or assets, and do not diagnose a reported defect: the Executor reads them anyway. Exception: a question for the owner, a GDD conflict or an acceptance check depends on a fact only the code gives. Then make at most 3 targeted reads, in one message, and say why in `gdd_check`.

## 3. Plan
Global node: the owner's words, goal, acceptance, GDD check, questions. Add subtasks only when one request spans several systems or roles. A subtask is a checkpoint with its own commit: one role, one outcome, a testable `done_when`, its inputs. Never write file names, functions or steps the Executor can decide. Order subtasks so that every dependency comes first.

- Large batch: keep one global and split it into bounded slices of the same role, with disjoint scope and checks (about 4–5 test families per C++ slice). No tightly coupled splits, no per-file microtasks.
- Editor work: one main asset per bp subtask (owner 2026-10-14). Order: materials, then Niagara and widgets, then the Blueprint that wires them, then map placement.
- Doc: a global with cpp or bp subtasks ends with one subtask `agent: doc` whose `inputs` are the `sys-*`, `bp-*` and tool nodes to create or update (owner 2026-10-27).
- Visual check: one subtask `agent: capture` whose `done_when` is a checklist C1..Cn taken from `acceptance`, each point observable on a frame (owner 2026-10-29). For a screen or widget, the default stays the four resolutions of [[umg-layout]]: fewer only if the owner chooses it, asked as a question whose default is the four.
- Visuel family, preview gate (owner 2026-11-11, [[task-501-visual-preview-gate]]): the first subtask is one preview: a mockup when the look is not decided, otherwise one single in-game slice, with one capture. Its `done_when` ends with the owner go; every build subtask depends on it.
- Uncertain integration that is not a matter of taste (owner 2026-11-01): plan one representative slice, for example one shop card with its states or one clip in a game state, then its check, then expansion subtasks that depend on it. Record the expected result, references and rules to preserve. A passing slice needs no owner go; a look or taste to find takes the preview gate above.

- Verification level (owner 2026-11-06): set `level` per global from the criteria of the request ([[verification-levels]]), standard when in doubt, and size `acceptance` and the capture checklist to it.

Plan only what the owner asked: no new tests, bot runs, screenshot sessions or features. Only update existing tests that a requested change breaks. The plan shown lists exactly what will run.

## 4. Confirm
Show the plan once, with the verification level of each global. A one-word answer (leger, standard, exploratoire) changes that level, nothing else; with several globals he names which. A "no" to every proposed option means no task: ask or wait, never relaunch on a reinterpreted target. The owner confirms the global task; subtasks then run without further confirmation. A second global task requested meanwhile waits for its own confirmation.

Status and defaults file: [[task-spec]].

## Relations
- [[agents-index]] — parent index.
- [[execution-protocol]] — steps 5 and 6, the Executor's procedure.
- [[task-spec]] — how the plan is written.
- [[orchestrator-lessons]] — owner rules: read before every plan.
- [[agent-planner]] — runs steps 0 to 4.
- [[agent-executor]] — runs what it plans.
- [[tasks-to-do-index]] — read at step 0.
- [[umg-layout]] — the resolutions a screen is checked at.
- [[verification-levels]] — the level set at step 3 and shown at step 4.
- [[request-families]] — entry routing that sends a request here or past the Planner.
- [[task-501-visual-preview-gate]] — owner decision behind the preview gate of step 3.
