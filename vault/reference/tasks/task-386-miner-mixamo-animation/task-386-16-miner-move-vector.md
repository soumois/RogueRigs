---
id: task-386-16-miner-move-vector
description: "Cpp: URRMinerAnimInstance exposes MoveX and MoveY, ground velocity in the aim frame over max speed, clamped to length 1, with a debug log"
parent: task-386-miner-mixamo-animation
kind: task
status: done
---
# Miner animated with the owner Mixamo clips: movement vector

- goal: Add MoveX = dot(velocity, forward)/maxSpeed and MoveY = dot(velocity, right)/maxSpeed (pawn yaw follows the aim), clamped to length 1, as BlueprintReadOnly; keep Direction, BlendSpeed, LocomotionPlayRate for now (ABP still reads them); LocomotionPlayRate 1 for now; add a toggleable debug log (console variable) of MoveX, MoveY, GroundSpeed.
- agent: cpp
- depends_on: task-386-15-abp-palier3-directions
- inputs: Owner words 2026-10-30: simple blend space, direction as a vector (X forward/back, Y left/right), vector length = intensity (1 = max speed 600 uu/s); delete A_Miner_WalkBwd (bad clip); samples Back (-1,0) A_Miner_RunBwd, Fwd (1,0) A_Miner_RunFwd, Left (0,-1) A_Miner_RunLeft, Right (0,1) A_Miner_RunRight; idle stays in the Idle/Run state machine, thresholds unchanged; Source/RogueRigs URRMinerAnimInstance (.h/.cpp), sys-player-classes doc, global request
- expected_output: change summary, build result, commit hash
- done_when: builds with the editor closed, logic in C++ only, no new test, committed by path as task-386-16-miner-move-vector: <summary>
- editor_state: closed
- token_ceiling: 0.4 M units
- model: opus
- tokens: input 26, cache write 35798, cache read 250928, output 9250
- usage: turns 12, context start 12k peak 36k, resumes 1 (message x1), 3 min, units 0.12 M
- runs: 2
- milestone: M2
- started: 2026-10-30T16:39:53Z
- waiting_for: none
- result: MoveX/MoveY (velocity in actor frame / MaxWalkSpeed, clamped to 1), LocomotionPlayRate fixed 1, rr.Debug.MinerAnim log; build ok; commit f6dccd5c; first run blocked by owner editor open, resumed after closing it

## Relations
- [[task-386-miner-mixamo-animation]] — parent global task.