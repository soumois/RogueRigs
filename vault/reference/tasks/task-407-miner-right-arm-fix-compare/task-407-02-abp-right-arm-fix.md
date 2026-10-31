---
id: task-407-02-abp-right-arm-fix
description: "Bp: ABP_Miner: right arm correction from the two offsets, after the upper body layer, before the left hand IK"
parent: task-407-miner-right-arm-fix-compare
kind: task
status: cancelled
---
# Miner right arm correction on the idle pose, compared on one frame: bp

- goal: ABP_Miner raises the right elbow by RightElbowOffset and moves the right hand by RightHandOffset on the upper body pose, method Q5, before the left hand IK so the left hand follows the rifle.
- agent: bp
- depends_on: task-407-01-right-arm-offsets-cvars
- inputs: bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), task-407-01-right-arm-offsets-cvars (vault/reference/tasks/task-407-miner-right-arm-fix-compare/task-407-01-right-arm-offsets-cvars.md), task-406-02-abp-upper-body-layer (vault/reference/tasks/task-406-miner-upper-body-idle-layer/task-406-02-abp-upper-body-layer.md), Tools/unreal/mcp-tool-sheet.md
- expected_output: Change summary: nodes added and settings (spaces, chain, any virtual bone on SKEL_Miner), readings, unverified items, commit hash. No vault doc edit.
- done_when: AnimGraph reads back: ... Layered blend per bone -> LocalToComponent -> right arm correction -> left hand Two Bone IK -> ComponentToLocal -> Output; compiled and saved; readings (ABP preview or PIE, standing): offsets (0,0,0) -> RightHand within 0.5 uu of the pose without the correction; RightHandOffset (0,5,0) -> RightHand about 5 uu further along +Y (within 1 uu); RightElbowOffset (0,0,5) -> RightForeArm about 5 uu higher, RightHand within 1 uu; in PIE with the rifle, left hand within 3 uu of LeftHandGrip; anything not read is reported unverified; changed assets committed by path `task-407-02-abp-right-arm-fix: ...`.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-31T07:30:08Z
- waiting_for: none
- result: open

## Relations
- [[task-407-miner-right-arm-fix-compare]] — parent global task.
