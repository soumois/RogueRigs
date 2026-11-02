---
id: task-417-02-doc
description: "Doc: update sys-camera-shake and sys-player for the hit shake"
parent: task-417-camera-shake-on-damage
kind: task
status: done
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
- model: sonnet
- tokens: input 10, cache write 35630, cache read 100638, output 3451
- usage: turns 5, context start 11k peak 36k, resumes 0, 1 min, units 0.07 M
- runs: 1
- milestone: M3
- started: 2026-11-02T18:34:11Z
- waiting_for: none
- result: sys-camera-shake, sys-player, sys-player-classes updated with URRHitCameraShake, HitShakeClass and trigger; lint clean; commit 62efb380

## Relations
- [[task-417-camera-shake-on-damage]] — parent global task.
