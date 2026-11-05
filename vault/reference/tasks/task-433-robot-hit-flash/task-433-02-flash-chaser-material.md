---
id: task-433-02-flash-chaser-material
description: "Bp: Representative slice: the chaser reads the flash value and turns plain white, baked animation intact."
parent: task-433-robot-hit-flash
kind: task
status: done
---
# Robot hit flash: bp

- goal: Representative slice: the chaser reads the flash value and turns plain white, baked animation intact.
- agent: bp
- depends_on: task-433-01-flash-trigger
- inputs: readability-rules (vault/reference/gdd/art-audio/readability-rules.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md)
- expected_output: see done_when
- done_when: Chaser material compiled with no error, reads the flash slot, plain white at 1 and unchanged at 0; animation data slots untouched; change readable by the next subtask as the pattern for the other classes.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-433
- usage: included in the executor run of task-433
- runs: 1
- milestone: M2
- started: 2026-11-05T16:13:51Z
- waiting_for: none
- result: New MF_RobotHitFlash: HitFlash scalar on CPD 2 (default 0) x HitFlashStrength (1); base colour lerps to black, emissive lerps to HitFlashEmissive 3 (flat white, provisional). Wired between the texture and the outputs of M_RobotMeshy, the master of MI_ChaserV2_Body (the chaser the game spawns); WPO pin untouched, CPD 0 and 1 unread by it. No compile error in the log. Look unverified: no capture in this task, owner judges.

## Relations
- [[task-433-robot-hit-flash]] — parent global task.
