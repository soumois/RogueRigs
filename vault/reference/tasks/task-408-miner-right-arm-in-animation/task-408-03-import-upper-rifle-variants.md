---
id: task-408-03-import-upper-rifle-variants
description: "Bp: import A_Miner_UpperRifle_V1..V3 on SKEL_Miner, ABP_Miner upper body set to V2, compiles after the restore"
parent: task-408-miner-right-arm-in-animation
kind: task
status: done
---
# Miner right arm corrected in the animation, variants compared on one frame: bp

- goal: The three new animations are AnimSequences on SKEL_Miner next to the A_Miner clips; ABP_Miner (restored to task-406) plays the locomotion blend space on the legs only and the created animation on the upper body: UpperBodySequence class default = A_Miner_UpperRifle_V2 (middle proposal, provisional until the owner picks); left hand IK unchanged.
- agent: bp
- depends_on: task-408-01-remove-arm-offsets-upper-pose-cvar, task-408-02-upper-rifle-variants
- inputs: bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), task-386-04-miner-clips-import (vault/reference/tasks/task-386-miner-mixamo-animation/task-386-04-miner-clips-import.md), Tools/unreal/mcp-tool-sheet.md, Art/Meshes/Miner/Anims/A_Miner_UpperRifle_V1.fbx (and V2, V3)
- expected_output: Change summary: three assets with paths, import settings, read-back length and frame count, ABP_Miner graph read-back and class default, compile result, commit hash. No vault doc edit.
- done_when: /Game/RogueRigs/Characters/Miner/Anims/A_Miner_UpperRifle_V1..V3 exist on SKEL_Miner (no new skeleton), imported with the task-386-04 settings (scale, root motion off, as A_Miner_Idle), length and frame count equal to A_Miner_Idle; ABP_Miner graph reads back Locomotion -> Layered blend per bone (branch filter Spine, depth 0, BlendPoses_0 fed by UpperBodySequence) -> LocalToComponent -> left hand Two Bone IK -> ComponentToLocal -> Output, no right arm correction node; UpperBodySequence class default = A_Miner_UpperRifle_V2; compiles without error; assets and ABP_Miner saved and committed by path `task-408-03-import-upper-rifle-variants: ...`.
- editor_state: open
- token_ceiling: 0.6 M units
- model: sonnet
- tokens: input 64, cache write 74411, cache read 1535475, output 11964
- usage: turns 32, context start 12k peak 74k, resumes 0, 2 min, units 0.31 M
- runs: 1
- milestone: M2
- started: 2026-10-31T07:47:36Z
- waiting_for: none
- result: V1..V3 imported on SKEL_Miner (93 frames, 3.1 s as idle), ABP_Miner default V2, binding via Get UpperBodySequence verified, compiles; commit a208103f; branch filter and depth not read back; no PIE check

## Relations
- [[task-408-miner-right-arm-in-animation]] — parent global task.
