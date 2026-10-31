---
id: task-401-03-abp-left-hand-ik
description: "Bp: ABP_Miner: Two Bone IK on the left arm onto LeftHandGrip"
parent: task-401-miner-hands-fit-rifle
kind: task
status: done
---
# Miner hands fitted to the burst rifle: bp

- goal: ABP_Miner: after the locomotion pose, a Two Bone IK on the left hand bone (left arm and forearm), effector LeftHandIKTarget in the right hand bone space, joint target placing the elbow down and out, alpha LeftHandIKAlpha; nothing else in the graph changes.
- agent: bp
- depends_on: task-401-01-left-hand-ik-target, task-401-02-rifle-left-grip-socket
- inputs: bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), task-386-miner-mixamo-animation (vault/reference/tasks/task-386-miner-mixamo-animation.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), `/Game/RogueRigs/Characters/Miner/ABP_Miner`, `/Game/RogueRigs/Characters/Miner/Anims/BS_Miner_Locomotion` (owner edit 5e8a0735, do not change)
- expected_output: ABP_Miner compiled and saved; change summary (node, bones, spaces, joint target); PIE read-back of the left hand to LeftHandGrip distance; commit `task-401-03-abp-left-hand-ik: ...`.
- done_when: ABP_Miner compiles; in PIE standing and with rr.Debug.MinerAnimForce on each of the four directions, the left hand bone is within 3 uu of the rifle LeftHandGrip socket; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: sonnet
- tokens: input 116, cache write 114786, cache read 6634845, output 34313
- usage: turns 58, context start 59k peak 159k, resumes 0, 12 min, units 0.98 M
- runs: 1
- milestone: M2
- started: 2026-10-31T05:41:48Z
- waiting_for: none
- result: ABP_Miner Two Bone IK on LeftHand after locomotion, effector LeftHandIKTarget in RightHand bone space, alpha LeftHandIKAlpha, joint target (45,10,95) comp space, stretch on; PIE hand-to-grip 0.00001 uu standing and four forced directions; elbow only numerically checked; commit d82f5f9f.

## Relations
- [[task-401-miner-hands-fit-rifle]] — parent global task.
