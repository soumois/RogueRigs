---
id: task-405-01-joint-target-diagnose-fix
description: "Bp: ABP_Miner: diagnose the no-effect joint target and fix it inside the ABP"
parent: task-405-left-elbow-joint-target
kind: task
status: todo
---
# Left elbow follows the Two Bone IK joint target: bp

- goal: Find why Joint Target Location on the ABP_Miner Two Bone IK changes nothing and make the left elbow follow it, within the scope of Q4 (read the answer in the global request). Check in order: node joint target space and bone, pin exposed or bound, alpha in the ABP editor preview (no held weapon there), stretch settings, PIE distance shoulder (LeftArm) to LeftHandGrip against LeftArm + LeftForeArm length, elbow (LeftForeArm) world position for joint targets (45, 10, 95) and (-45, 10, 95) or any pair 40 cm apart. Keep the effector, alpha and the rest of the graph; BS_Miner_Locomotion untouched.
- agent: bp
- depends_on: task-404-01-rifle-mesh-transform
- inputs: bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), burst-rifle-mesh (vault/reference/gdd/art-audio/burst-rifle-mesh.md), task-401-03-abp-left-hand-ik (vault/reference/tasks/task-401-miner-hands-fit-rifle/task-401-03-abp-left-hand-ik.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), `/Game/RogueRigs/Characters/Miner/ABP_Miner`, `/Game/RogueRigs/Characters/Miner/Anims/BS_Miner_Locomotion` (owner edit 5e8a0735, do not change), debug command `rr.Debug.MinerAnimForce "x y"`, Global request, gdd_check and Questions of task-405-left-elbow-joint-target
- expected_output: Cause stated with the measured values (arm length, shoulder-to-grip distance, elbow positions, node settings); ABP_Miner compiled and saved when fixed; change summary; commit `task-405-01-joint-target-diagnose-fix: ...`. When the cause is outside Q4 scope: no asset change, the subtask and global set wait-answers with the cause and options.
- done_when: Cause recorded in result; ABP_Miner compiles; PIE standing and in the four forced directions: elbow positions at least 5 cm apart for the two joint targets, left hand within 3 uu of LeftHandGrip; joint target value left at the provisional default (or the one that bends the elbow down and out) for the owner to tune; committed by path.
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
- [[task-405-left-elbow-joint-target]] — parent global task.
