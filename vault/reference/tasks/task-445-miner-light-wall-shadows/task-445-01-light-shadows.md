---
id: task-445-01-light-shadows
description: "Bp: FloorWallLight casts shadows, its position and intensity left as the owner set them."
parent: task-445-miner-light-wall-shadows
kind: task
status: done
---
# Miner light casts wall shadows only: bp

- goal: FloorWallLight casts shadows, its position and intensity left as the owner set them.
- agent: bp
- depends_on: none
- inputs: bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), task-425-miner-floor-wall-light (vault/reference/tasks/task-425-miner-floor-wall-light.md), visual-style (vault/reference/gdd/art-audio/visual-style.md), lights-no-shadows (vault/reference/tech/blueprints/world/lights-no-shadows.md), sys-grid (vault/reference/tech/systems/world/sys-grid.md)
- expected_output: see done_when
- done_when: Compiled and saved; castShadows on; height and intensity read back, equal to their values before this subtask and given in the report; lighting channels unchanged; no unsaved editor change of the owner lost.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-445
- usage: included in the executor run of task-445
- runs: 1
- milestone: M2
- started: 2026-11-05T21:12:19Z
- waiting_for: none
- result: BP_Miner FloorWallLight castShadows false to true, compiled with warnings as errors, saved. Read back identical before and after: location (0,0,0), intensity 4, attenuation 800, falloff exponent 4, channels ch1 only, Movable. Editor had no owner session: launched by the Executor on a clean tree.

## Relations
- [[task-445-miner-light-wall-shadows]] — parent global task.
