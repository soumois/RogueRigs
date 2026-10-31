---
id: task-406-02-abp-upper-body-layer
description: "Bp: ABP_Miner: Layered blend per bone, legs from locomotion, upper body from UpperBodySequence (A_Miner_Idle)"
parent: task-406-miner-upper-body-idle-layer
kind: task
status: done
---
# Miner upper body from the idle rifle pose over the locomotion legs: bp

- goal: ABP_Miner plays the locomotion state machine on the legs and UpperBodySequence (class default A_Miner_Idle, looping) on the upper body from Spine, before the left hand IK (Q1, Q2, Q3).
- agent: bp
- depends_on: task-406-01-upper-body-sequence-prop
- inputs: bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), task-406-01-upper-body-sequence-prop (vault/reference/tasks/task-406-miner-upper-body-idle-layer/task-406-01-upper-body-sequence-prop.md), Tools/unreal/mcp-tool-sheet.md, Content/RogueRigs/Characters/Miner/ABP_Miner.uasset (start from the working-tree file: owner edits, uncommitted)
- expected_output: Change summary: what the graph held before (owner edits), nodes added and their settings (bone name read on SKEL_Miner, depth, mesh space rotation blend, sequence binding), class default set, PIE readings, what stays unverified, commit hash. No vault doc edit.
- done_when: First report the current AnimGraph (owner edits); if a Layered blend per bone exists, complete it instead of adding a second. AnimGraph reads back: Locomotion state machine -> Layered blend per bone (base = locomotion; blend pose 0 = looping Sequence Player bound to UpperBodySequence; branch filter Spine, depth 0; mesh space rotation blend on) -> LocalToComponent -> left hand Two Bone IK -> ComponentToLocal -> Output; ABP_Miner class default UpperBodySequence = A_Miner_Idle; compiled without error and saved; owner edits kept; PIE standing then moving (through the cheat settings or player input, else reported unverified): arm bones keep the idle pose relative to Spine (RightHand, LeftHand within 5 uu) while the feet move, left hand within 3 uu of LeftHandGrip; BS_Miner_Locomotion and the A_Miner clips not modified; ABP_Miner.uasset committed by path `task-406-02-abp-upper-body-layer: ...`.
- editor_state: open
- token_ceiling: 1.0 M units
- model: sonnet
- tokens: input 52, cache write 75569, cache read 1298428, output 15482
- usage: turns 26, context start 12k peak 76k, resumes 0, 2 min, units 0.30 M
- runs: 1
- milestone: M2
- started: 2026-10-31T07:09:37Z
- waiting_for: none
- result: Layered blend per bone (Spine, depth 0, mesh space rotation on) + looping sequence player on UpperBodySequence added in ABP_Miner, class default A_Miner_Idle, compiled and saved (b03e520d). PIE bone check not run: unverified. Unconnected owner TwoBoneIK_0 left untouched.

## Relations
- [[task-406-miner-upper-body-idle-layer]] — parent global task.
