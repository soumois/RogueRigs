---
id: task-405-01-abp-right-elbow-ik
description: "Bp: ABP_Miner: right elbow raised by an owner offset, hand kept in place"
parent: task-405-right-elbow-raise
kind: task
status: cancelled
---
# Miner right elbow raised a little, owner tunable: bp

- goal: Apply the Q4 method (read the answer in the global request; default A): in ABP_Miner, after the Locomotion pose and before the left hand Two Bone IK, add the right elbow adjustment driven by a new instance-editable vector variable `RightElbowOffset` (cm, default per Q4, provisional). With A: Two Bone IK on RightHand, effector Bone Space RightHand location 0, joint target = RightForeArm position plus RightElbowOffset in a space where +Z is up and the offset points away from the body (state the axes in the summary), stretching off. The left hand IK, its variables and BS_Miner_Locomotion stay unchanged.
- agent: bp
- depends_on: task-404-01-rifle-mesh-transform
- inputs: bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), task-401-03-abp-left-hand-ik (vault/reference/tasks/task-401-miner-hands-fit-rifle/task-401-03-abp-left-hand-ik.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), `/Game/RogueRigs/Characters/Miner/ABP_Miner`, `/Game/RogueRigs/Characters/Miner/Anims/BS_Miner_Locomotion` (owner edit 5e8a0735, do not change), debug command `rr.Debug.MinerAnimForce "x y"`, Global request, gdd_check and Questions of task-405-right-elbow-raise
- expected_output: ABP_Miner compiled and saved; change summary (node, bones, spaces, variable, default, axis meaning); PIE read-backs; commit `task-405-01-abp-right-elbow-ik: ...`.
- done_when: ABP_Miner compiles; PIE standing and in the four forced directions: right hand moves less than 0.5 uu versus RightElbowOffset zero (Q4 A), the right elbow is further from the spine than with the offset zero, two offset values 10 cm apart give elbow positions at least 3 cm apart, left hand within 3 uu of LeftHandGrip; RightElbowOffset editable in the ABP class defaults; committed by path.
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
- [[task-405-right-elbow-raise]] — parent global task.
