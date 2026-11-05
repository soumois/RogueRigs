---
id: task-445-01-light-shadows
description: "Bp: FloorWallLight casts shadows and sits just above the wall tops, its pool kept by retuned values."
parent: task-445-miner-light-wall-shadows
kind: task
status: todo
---
# Miner light casts wall shadows only: bp

- goal: FloorWallLight casts shadows and sits just above the wall tops, its pool kept by retuned values.
- agent: bp
- depends_on: none
- inputs: bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), task-425-miner-floor-wall-light (vault/reference/tasks/task-425-miner-floor-wall-light.md), visual-style (vault/reference/gdd/art-audio/visual-style.md), lights-no-shadows (vault/reference/tech/blueprints/world/lights-no-shadows.md), sys-grid (vault/reference/tech/systems/world/sys-grid.md)
- expected_output: see done_when
- done_when: Compiled and saved; castShadows on, height and intensity read back, lighting channels unchanged.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-445-miner-light-wall-shadows]] — parent global task.
