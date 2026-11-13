---
id: task-527-04-doc
description: "Doc: sys-audio: the objective complete event"
parent: task-527-audio-zone-objective-complete
kind: task
status: done
---
# Son à la validation d'un coffre ou d'une console: doc

- goal: sys-audio: the objective complete event
- agent: doc
- depends_on: task-527-03-complete-event
- inputs: sys-audio (vault/reference/tech/systems/core/sys-audio.md), sys-objectives-console-chest (vault/reference/tech/systems/world/sys-objectives/sys-objectives-console-chest.md)
- expected_output: see done_when
- done_when: A4 doc part holds; lint clean; commit.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-527
- usage: included in the executor run of task-527
- runs: 1
- milestone: M2
- started: 2026-11-13T16:22:45Z
- waiting_for: none
- result: sys-audio: ObjectiveComplete call site, row and fade (200/1075 uu, no near reduction), -game check, relations to task-527 and sys-objectives-console-chest; sys-audio-distance-sounds: user ObjectiveComplete; sys-objectives-console-chest: CompleteSoundEvent in CompleteHack. Lint clean. Commit of the doc before this closing.

## Relations
- [[task-527-audio-zone-objective-complete]] — parent global task.
