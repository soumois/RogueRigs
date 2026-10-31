---
id: task-403-burst-rifle-material-fix
description: "Global task: diagnose in the editor why SM_BurstRifle renders flat cream-white in game, fix M_BurstRifle or its textures, check by capture."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Burst rifle material fix: colours readable in game

- request: Owner, 2026-10-31, verbatim (French, relayed by the main session): "Lance une correction". Context relayed by the main session: the owner had asked what is wrong with the burst rifle material. In capture `Saved/Captures/RifleInHand2` (crop_still-b.png) SM_BurstRifle renders as a flat cream-white silhouette in game, while its base colour map (`Art/Source/burst-rifle/meshy/burst_rifle_meshy_basecolor.png`) is a teal/rust/dark/cream atlas and the miner next to it is shaded normally. M_BurstRifle compile and wiring were never confirmed (task-400-02); Metallic and Roughness were imported sRGB off (TC_Masks), a bp agent choice. Cause not diagnosed; hypotheses: BaseColor not wired or material not compiled, Metallic/Roughness wiring, UV, lighting. Reading: one point, a fix of the rifle look only; game rules, socket, scale and shot origin unchanged. Owner answer 2026-10-31 (relayed by the main session, verbatim): "Ok" = the plan and the defaults of Q1, Q2, Q3 accepted.
- goal: In game SM_BurstRifle shows its base colour map (teal, rust, dark barrel, cream plates) and is shaded like the miner, with the cause of the flat cream-white look named and fixed.
- acceptance: The cause is named with editor evidence (material graph pins, compile result, texture settings, slot material, UV channels); M_BurstRifle compiles without error and is saved; the fix is in the rifle assets of `/Game/RogueRigs/Weapons/BurstRifle/` (or the RifleMesh override of BP_BurstRifle) and committed by path; a -game capture reports C1..C3 (teal, rust and dark areas readable; shaded like the miner; miner unchanged); doc nodes updated by the doc step; lint clean; committed by path.
- gdd_check: Read 2026-10-31: [[burst-rifle]] (spec approved; look: white + teal + rust salvaged plates, dark gunmetal grip, barrel and muzzle; Meshy maps accepted, task-395 rebake cancelled), [[burst-rifle-mesh]] (four 2048 maps, Unreal assets of task-400-02: M_BurstRifle 4 samples, BaseColor sRGB, Normal, Metallic and Roughness sRGB off TC_Masks; material compile unverified), [[bp-burst-rifle]] (RifleMesh = SM_BurstRifle; open observation: renders flat cream-white in RifleInHand2), [[task-400-burst-rifle-in-hand]] (done; capture C1 ambiguous for this reason). Conflicts: none. Open values the work needs: none in the GDD; the fix scope (Q1), the mask texture settings (Q2) and the capture (Q3) are asked. Out of scope: left-hand IK ([[task-401-miner-hands-fit-rifle]], separate, todo, not started). Milestone M2 (art).
- confirmed_by_owner: yes (2026-10-31, "Ok")
- milestone: M2
- next_step: task-403-02-burst-rifle-colours-capture
- started: 2026-10-31T04:33:01Z
- waiting_for: none
- result: open

## Questions
Decided 2026-10-31 (owner "Ok"): every default below is accepted; values provisional, owner 2026-10-31.
- Q1 Fix scope: Default: the bp agent first diagnoses in the editor (M_BurstRifle graph and pins, compile log, sampler type of each texture against its compression, material on the SM_BurstRifle slot and any override on the BP_BurstRifle RifleMesh, UV0 present, a lit preview), writes the cause, then fixes it in the rifle assets only: M_BurstRifle, the four T_BurstRifle textures, a SM_BurstRifle reimport if the UVs are lost, or the RifleMesh material override. If the cause is outside those (scene lighting, post process, C++), it stops before changing anything and the question comes back to you.
- Q2 Metallic and Roughness maps: Default: they stay two separate grey maps, sRGB off (masks), the standard setting for data maps; changed only if the diagnosis shows they cause the flat look. Provisional, owner 2026-10-31.
- Q3 Visual check: Default: a -game capture by the capture agent with the RifleInHand2 framing (1920x1080, camera arm 350), checklist C1 teal, rust and dark areas readable on the rifle, C2 the rifle shaded like the miner, C3 the miner unchanged; frames sent to you. Or you check it yourself in game and the capture subtask is removed.

## Subtasks
1. task-403-01-burst-rifle-material-diagnose-fix (vault/reference/tasks/task-403-burst-rifle-material-fix/task-403-01-burst-rifle-material-diagnose-fix.md) — bp: diagnose in the editor why SM_BurstRifle renders flat cream-white and fix M_BurstRifle or its textures
2. task-403-02-burst-rifle-colours-capture (vault/reference/tasks/task-403-burst-rifle-material-fix/task-403-02-burst-rifle-colours-capture.md) — capture: game capture: the rifle shows teal, rust and dark areas and is shaded like the miner
3. task-403-03-doc-burst-rifle-material-fix (vault/reference/tasks/task-403-burst-rifle-material-fix/task-403-03-doc-burst-rifle-material-fix.md) — doc: update bp-burst-rifle and burst-rifle-mesh with the cause and the fix

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-403-01-burst-rifle-material-diagnose-fix]] — step 1, bp.
- [[task-403-02-burst-rifle-colours-capture]] — step 2, capture.
- [[task-403-03-doc-burst-rifle-material-fix]] — step 3, doc.
- [[burst-rifle-mesh]] — mesh, maps and Unreal assets whose material is fixed.
- [[bp-burst-rifle]] — the Blueprint showing the rifle; holds the open colour observation.
- [[task-400-burst-rifle-in-hand]] — task that imported the rifle and produced the capture showing the defect.
