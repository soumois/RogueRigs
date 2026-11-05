---
id: task-435-02-remove-materials
description: "Bp: The robot materials and instances no longer read the flash and the flash function is deleted."
parent: task-435-remove-robot-hit-flash
kind: task
status: done
---
# Remove the robot hit flash: bp

- goal: The robot materials and instances no longer read the flash and the flash function is deleted.
- agent: bp
- depends_on: task-435-01-remove-trigger
- inputs: bp-robot-hit-flash (vault/reference/tech/blueprints/enemies/bp-robot-hit-flash.md), bp-robot-meshy-material (vault/reference/tech/blueprints/enemies/bp-robot-meshy-material.md), bp-robot-eye-material (vault/reference/tech/blueprints/enemies/bp-robot-eye-material.md), bp-shield-chaser-v2 (vault/reference/tech/blueprints/enemies/bp-shield-chaser-v2.md)
- expected_output: see done_when
- done_when: Masters recompile with no error and baked animation inputs intact; shield, emblem and kamikaze core instances keep their look; the function asset no longer exists and nothing references it.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-435
- usage: included in the executor run of task-435
- runs: 1
- milestone: M2
- started: 2026-11-05T17:32:23Z
- waiting_for: none
- result: M_RobotMeshy, M_RobotEye, M_ShieldChaserV2_Anim, MI_ShieldChaserV2_Shield and _Emblem restored from c5afc7f8 (no later change), MF_RobotHitFlash deleted. Masters recompiled and saved, WPO and MF_RobotAnimWPO read back, shield, emblem and core instances read back, no reference found; RogueRigs.Enemies passes.

## Relations
- [[task-435-remove-robot-hit-flash]] — parent global task.
