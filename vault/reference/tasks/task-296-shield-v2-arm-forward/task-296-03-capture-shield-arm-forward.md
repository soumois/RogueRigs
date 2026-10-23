---
id: task-296-03-capture-shield-arm-forward
description: "Capture the shield chaser v2 running with the shield forward, compared with the task-291 shots (cpp)."
parent: task-296-shield-v2-arm-forward
kind: task
status: done
---
# Capture the shield held forward

- goal: Capture in -game the shield chaser v2 running and bashing, side and top view if possible, compared with Saved/RobotMeshes/v1-clips/.
- agent: cpp
- depends_on: task-296-02-bp-shield-arm-forward
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md); files: vault/reference/tasks/task-289-robot-run-v1-motion/task-289-06-capture-run.md (capture recipe), Tools robot_shots.ps1
- expected_output: Saved/RobotMeshes/shield-forward/ shots and sheet, short description
- done_when: shield visibly ahead of the feet, arm forward, parts joined; no untracked throwaway files
- editor_state: any (runs beside the owner editor)
- token_ceiling: 30000
- model: opus
- tokens: input 32, cache write 45961, cache read 421881, output 9451
- runs: 1
- milestone: M2
- result: Shield chaser v2 forearm about horizontal, hand on the shield, shield +33 cm forward on run (+27 to +32 bash), min shield-leg 43.6 cm run / 34.8 bash (was 2.64 / 6.34); new SM_ShieldChaserV2_Meshy_AnimFwd and T_ShieldChaserV2AnimFwd on BP_ShieldChaserV2, AnimV1Clips kept; plain rows unchanged; renders Saved/Renders/task-296/, -game captures Saved/RobotMeshes/shield-forward/ (side run shows shield ahead of the feet, hand joined); unverified: bash pose in game, bake error 0.068 cm above 0.05 warning.

## Relations
- [[task-296-shield-v2-arm-forward]] — parent global task.
