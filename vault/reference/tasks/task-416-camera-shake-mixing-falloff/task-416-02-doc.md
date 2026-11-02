---
id: task-416-02-doc
description: "Doc: update sys-camera-shake for mixing and the located call"
parent: task-416-camera-shake-mixing-falloff
kind: task
status: todo
---
# Camera shakes mix and fade with distance: doc

- goal: Update the camera shake doc for the mixing rule, the cap and the located call, and remove the "heavy slam and explosions do not shake" line.
- agent: doc
- depends_on: task-416-01-shake-mixing-falloff
- inputs: task-416-camera-shake-mixing-falloff (vault/reference/tasks/task-416-camera-shake-mixing-falloff.md), sys-camera-shake (vault/reference/tech/systems/core/sys-camera-shake.md), agent-doc (vault/reference/agents/agent-doc.md)
- expected_output: sys-camera-shake updated (entry points, data, gotchas), lint clean, committed by path.
- done_when: sys-camera-shake describes the additive mixing, same-class restart, 12 uu cap and the located call with its falloff; the Q21 no-shake line replaced by a pointer to tasks 417 to 419; lint clean; committed.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-416-camera-shake-mixing-falloff]] — parent global task.
