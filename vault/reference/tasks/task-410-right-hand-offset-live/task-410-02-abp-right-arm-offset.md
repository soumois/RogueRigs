---
id: task-410-02-abp-right-arm-offset
description: "Bp: ABP_Miner: right arm Two Bone IK and RightHand rotation from the new inputs, before the left hand IK"
parent: task-410-right-hand-offset-live
kind: task
status: todo
---
# Live control of the right hand offset on the rifle grip: bp

- goal: In ABP_Miner AnimGraph, between LocalToComponent and the left hand Two Bone IK: add a Two Bone IK on IK bone RightHand (chain RightHand, RightForeArm, RightArm), effector location = `RightHandIKEffector` in Bone Space of bone RightHand, joint target location (0, 0, 0) in Bone Space of RightForeArm (elbow keeps its direction), stretching off, alpha = `RightHandOffsetAlpha`; then a Transform (Modify) Bone on RightHand, translation ignored, rotation mode Add in Bone Space = `RightHandRotationDelta`, alpha = `RightHandOffsetAlpha`; left hand Two Bone IK and everything else unchanged; compile and save. Names as given in the task-410-01 change summary.
- agent: bp
- depends_on: task-410-01-right-hand-offset-code
- inputs: task-410-right-hand-offset-live (vault/reference/tasks/task-410-right-hand-offset-live.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md)
- expected_output: ABP_Miner.uasset with the two right arm nodes wired, compiled and saved; one commit `task-410-02-abp-right-arm-offset: <summary>`; a change summary for the doc step (node order, settings, pins, commit hash).
- done_when: Graph read back: LocalToComponent -> right Two Bone IK (RightHand, effector RightHandIKEffector Bone Space RightHand, joint target 0 Bone Space RightForeArm, stretching off, alpha RightHandOffsetAlpha) -> Transform (Modify) Bone RightHand (rotation Add, Bone Space, RightHandRotationDelta) -> left Two Bone IK (unchanged) -> ComponentToLocal -> Output Pose; compile with no error; saved; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-410-right-hand-offset-live]] — parent global task.
