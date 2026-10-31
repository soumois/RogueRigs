---
id: task-403-01-burst-rifle-material-diagnose-fix
description: "Bp: diagnose in the editor why SM_BurstRifle renders flat cream-white and fix M_BurstRifle or its textures"
parent: task-403-burst-rifle-material-fix
kind: task
status: in-progress
---
# Burst rifle material fix: colours readable in game: bp

- goal: The cause of the flat cream-white rifle is named with editor evidence and fixed in the rifle assets; M_BurstRifle compiles and the rifle reads its colours in a lit editor view.
- agent: bp
- depends_on: none
- inputs: bp-burst-rifle (vault/reference/tech/blueprints/player/bp-burst-rifle.md), burst-rifle-mesh (vault/reference/gdd/art-audio/burst-rifle-mesh.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), task-400-02-burst-rifle-import (vault/reference/tasks/task-400-burst-rifle-in-hand/task-400-02-burst-rifle-import.md) (import settings and the bConvertSceneUnit fix used), Assets: `/Game/RogueRigs/Weapons/BurstRifle/` SM_BurstRifle, M_BurstRifle, T_BurstRifle_BaseColor, _Normal, _Metallic, _Roughness; `/Game/RogueRigs/Blueprints/Weapons/BP_BurstRifle` (RifleMesh), Source maps: `Art/Meshes/burst-rifle/T_BurstRifle_BaseColor.png`, `T_BurstRifle_Metallic.png`, `T_BurstRifle_Normal.png`, `T_BurstRifle_Roughness.png`, Look before: `Saved/Captures/RifleInHand2/crop_still-b.png` (rifle flat cream-white beside the normally shaded miner), `Saved/Captures/RifleInHand2/contact_sheet.png`, Reference: `Art/Source/burst-rifle/meshy/burst_rifle_meshy_basecolor.png` (teal, rust, dark, cream atlas), Blender renders `Saved/BurstRifleMesh/flat/result_textured_three_quarter.png`, `result_textured_left.png`, `result_textured_right.png` (not committed, expected look), Global request and Questions (fix scope Q1, mask settings Q2): `vault/reference/tasks/task-403-burst-rifle-material-fix.md`
- expected_output: Fixed and saved rifle assets, committed by path; a change summary for the doc step: the cause with its evidence, what changed (pins, texture settings, slot, reimport), compile result, and a lit editor screenshot path in `Saved/`.
- done_when: The summary names one cause with evidence read in the editor (graph pins, compile log, texture sRGB/compression/sampler type, slot material, UV channel count); M_BurstRifle compiles with no error and is saved; a lit editor view (viewport or asset preview screenshot in `Saved/`) shows teal, rust and dark areas on SM_BurstRifle; only rifle assets changed (Q1); committed by path. If the cause is outside the rifle assets, nothing is changed and the cause with evidence is reported for the owner.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-31T04:33:01Z
- waiting_for: none
- result: open

## Relations
- [[task-403-burst-rifle-material-fix]] — parent global task.
