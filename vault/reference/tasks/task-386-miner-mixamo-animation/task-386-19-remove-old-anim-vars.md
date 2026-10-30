---
id: task-386-19-remove-old-anim-vars
description: "Cpp: remove Direction, BlendSpeed and RunSpeed tunables of URRMinerAnimInstance that nothing uses"
parent: task-386-miner-mixamo-animation
kind: task
status: todo
---
# Miner animated with the owner Mixamo clips: cleanup of Direction and BlendSpeed

- goal: Remove what the simple blend space made useless if no asset reads it; keep a LocomotionPlayRate that is reasonable.
- agent: cpp
- depends_on: task-386-18-capture-simple-blendspace
- inputs: Owner words 2026-10-30: simple blend space, direction as a vector (X forward/back, Y left/right), vector length = intensity (1 = max speed 600 uu/s); delete A_Miner_WalkBwd (bad clip); samples Back (-1,0) A_Miner_RunBwd, Fwd (1,0) A_Miner_RunFwd, Left (0,-1) A_Miner_RunLeft, Right (0,1) A_Miner_RunRight; idle stays in the Idle/Run state machine, thresholds unchanged; URRMinerAnimInstance, ABP_Miner read-back that no pin uses them
- expected_output: change summary, build, commit
- done_when: builds, ABP still compiles (editor reopened to check compile or by log), committed by path as task-386-19-remove-old-anim-vars: <summary>
- editor_state: closed
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-386-miner-mixamo-animation]] — parent global task.