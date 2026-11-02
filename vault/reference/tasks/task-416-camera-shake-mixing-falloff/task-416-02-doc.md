---
id: task-416-02-doc
description: "Doc: update sys-camera-shake for mixing and the located call"
parent: task-416-camera-shake-mixing-falloff
kind: task
status: done
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
- model: sonnet
- tokens: input 10, cache write 21898, cache read 64458, output 3121
- usage: turns 5, context start 11k peak 22k, resumes 0, 1 min, units 0.05 M
- runs: 1
- milestone: M3
- started: 2026-11-02T18:22:24Z
- waiting_for: none
- result: Done, commit 7ee85d80: sys-camera-shake documents mixing, same-class restart, 12 uu cap, located call and falloff; Q21 line replaced by pointer to 417-419.

## Relations
- [[task-416-camera-shake-mixing-falloff]] — parent global task.
