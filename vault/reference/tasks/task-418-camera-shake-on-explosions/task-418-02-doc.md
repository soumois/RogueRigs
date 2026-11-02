---
id: task-418-02-doc
description: "Doc: update sys-explosions and sys-camera-shake for the explosion shake"
parent: task-418-camera-shake-on-explosions
kind: task
status: done
---
# Camera shake from nearby explosions: doc

- goal: Document the explosion shake and its per-source values.
- agent: doc
- depends_on: task-418-01-explosion-shake
- inputs: task-418-camera-shake-on-explosions (vault/reference/tasks/task-418-camera-shake-on-explosions.md), sys-explosions (vault/reference/tech/systems/combat/sys-explosions.md), sys-camera-shake (vault/reference/tech/systems/core/sys-camera-shake.md), agent-doc (vault/reference/agents/agent-doc.md)
- expected_output: sys-explosions (resolution order, data) and sys-camera-shake (callers, preset) updated, lint clean, committed by path.
- done_when: Both nodes state the explosion shake trigger, strengths, max distances and where they are tuned; lint clean; committed.
- editor_state: any
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 10, cache write 30752, cache read 88344, output 3536
- usage: turns 5, context start 11k peak 31k, resumes 0, 1 min, units 0.06 M
- runs: 1
- milestone: M3
- started: 2026-11-02T18:46:18Z
- waiting_for: none
- result: sys-explosions and sys-camera-shake updated for the explosion shake (trigger, strengths, max distances, tuning in ExplosionStats.csv); commit 7932dc64; lint OK

## Relations
- [[task-418-camera-shake-on-explosions]] — parent global task.
