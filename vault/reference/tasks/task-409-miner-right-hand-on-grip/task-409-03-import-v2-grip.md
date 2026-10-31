---
id: task-409-03-import-v2-grip
description: "Bp: import A_Miner_UpperRifle_V2Grip on SKEL_Miner, ABP_Miner UpperBodySequence set to it"
parent: task-409-miner-right-hand-on-grip
kind: task
status: todo
---
# Miner right hand on the rifle pistol grip, rifle kept where the owner placed it: bp

- goal: A_Miner_UpperRifle_V2Grip is an AnimSequence on SKEL_Miner next to V1..V3 and ABP_Miner plays it on the upper body (UpperBodySequence class default); graph otherwise unchanged.
- agent: bp
- depends_on: task-409-02-upper-rifle-v2-grip
- inputs: bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), task-408-03-import-upper-rifle-variants (vault/reference/tasks/task-408-miner-right-arm-in-animation/task-408-03-import-upper-rifle-variants.md), Tools/unreal/import_miner_upper_rifle.py, Tools/unreal/mcp-tool-sheet.md, Art/Meshes/Miner/Anims/A_Miner_UpperRifle_V2Grip.fbx
- expected_output: Change summary: asset path, import settings, read-back length and frame count, ABP_Miner class default, compile result, commit hash. No vault doc edit.
- done_when: /Game/RogueRigs/Characters/Miner/Anims/A_Miner_UpperRifle_V2Grip exists on SKEL_Miner (no new skeleton), imported with the settings of V1..V3, length and frame count equal to A_Miner_UpperRifle_V2; ABP_Miner UpperBodySequence class default = A_Miner_UpperRifle_V2Grip, graph unchanged otherwise; compiles without error; saved and committed by path `task-409-03-import-v2-grip: ...`.
- editor_state: open
- token_ceiling: 0.6 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-409-miner-right-hand-on-grip]] — parent global task.
