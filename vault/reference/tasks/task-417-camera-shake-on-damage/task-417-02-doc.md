---
id: task-417-02-doc
description: "Doc: update sys-camera-shake and sys-player for the hit shake"
parent: task-417-camera-shake-on-damage
kind: task
status: in-progress
---
# Camera shake when the miner takes damage: doc

- goal: Document the hit preset and its caller.
- agent: doc
- depends_on: task-417-01-hit-shake
- inputs: task-417-camera-shake-on-damage (vault/reference/tasks/task-417-camera-shake-on-damage.md), sys-camera-shake (vault/reference/tech/systems/core/sys-camera-shake.md), sys-player (vault/reference/tech/systems/core/sys-player.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), agent-doc (vault/reference/agents/agent-doc.md)
- expected_output: sys-camera-shake (callers, hit preset values) and sys-player or sys-player-classes (damage handler plays it) updated, lint clean, committed by path.
- done_when: Both nodes name the hit preset, its values and its trigger; lint clean; committed.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: 2026-11-02T18:34:11Z
- waiting_for: none
- result: open

## Relations
- [[task-417-camera-shake-on-damage]] — parent global task.
