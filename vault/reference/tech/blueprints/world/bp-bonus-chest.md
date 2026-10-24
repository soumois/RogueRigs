---
id: bp-bonus-chest
description: "BP_BonusChest, Blueprint of ARRBonusChest: flat-face chest from three meshes (Mesh body, Lid, Latch); open for its components, slots, pivots and events."
parent: blueprints-world-index
kind: doc
---
# BP_BonusChest

- asset_path: /Game/RogueRigs/Blueprints/Objectives/BP_BonusChest
- parent_class: ARRBonusChest (/Script/RogueRigs.RRBonusChest)
- purpose: the optional bonus chest dropped at 60 % alert by the run flow ([[sys-objectives]]); visuals only.
- components (task-306-03, owner 2026-10-24 provisional): hierarchy ConsoleRoot > Mesh > Lid > Latch, plus the inherited GroundTelegraph (zone circle, [[sys-ground-telegraph]]). Inherited `Mesh` = StaticMesh /Game/RogueRigs/Art/Chest/SM_Chest_Body, relative (0, 0, 0), rotation yaw 90, scale 1 (the engine cube and its scale 1.4 x 1 x 0.8, z 40 are gone), NoCollision, no dynamic shadow, no override materials. `Lid` (StaticMeshComponent, parent Mesh) = SM_Chest_Lid at relative (0, -31.137, 41.25) in Mesh space (the hinge axis, X), rotation 0, scale 1, NoCollision, bCastDynamicShadow false. `Latch` (StaticMeshComponent, parent Lid) = SM_Chest_Latch at relative (-0.475, 55.286, 10.238) in Lid space (its top edge, hangs on the lid front), rotation 0, scale 1, NoCollision, bCastDynamicShadow false. Task-308 (LockLight) and task-309 (lid, Interior) drive these by the names Mesh, Lid and Latch.
- slots_and_pivots: SM_Chest_Body slots [0] M_BonusChest (1809 faces), [1] Interior (301, inner cavity, task-309); SM_Chest_Lid slot [0] M_BonusChest; SM_Chest_Latch slots [0] M_BonusChest (419), [1] LockLight (9, task-308). All slots use /Game/RogueRigs/Art/Chest/M_Chest for now (base colour parameter BaseColorTex = T_Chest_BaseColor, Roughness 0.45, Metallic 0, no emissive input yet): LockLight until task-308, Interior until task-309; set them on the mesh assets or as component overrides. Pivots: body origin bottom centre, bounds x -65..65, y -33.6..34.1, z 0..41.25 (body, Mesh space); lid origin on the hinge, local bounds x -60.4..60.6, y -3.8..61.7, z 0..24.3; latch origin on its top edge, local bounds x +-12.5, y -1.1..10.8, z -43.9..0. Assembled about 130 x 70 x 66 uu; footprint stays 2 x 2 cells (134 uu square, set in C++).
- orientation: the FBX import mirrors Y (as for the console, [[sys-objectives-console-chest]]): the lock face of the Blender frame (-Y) is world +Y at yaw 0 and the hinge is at -Y; the lid opens about the X axis. The spawner uses rotation zero and the miner camera yaw is 0 (looks along +X, pitch -50, camera on the -X side), so +Y would be a side face (screen right). Mesh therefore has relative yaw 90: lock face toward -X, toward the camera, long side along world Y; the footprint is square so it is unchanged. Sign of the opening roll about X not measured (task-309 checks it in game: the lid extends toward +Y of Mesh space, so opening it up is the rotation that lifts +Y into +Z).
- exposed_defaults: C++ defaults kept (hack 8 s, zone 6 cells, reward values). Set as `BonusChestClass` in `RunFlowConfig` of [[bp-rr-game-mode]] (compile BP_RRGameMode after editing it, else a PIE reads None).
- events: OnHackProgress(Progress, bHacking) and OnRewardGranted(XP, OreChunks, bUpgradeChoiceQueued) are now empty events (their GlowAmount SetScalarParameterValueOnMaterials calls were removed by task-306-03, default Q5; M_Chest has no GlowAmount parameter, MI_GB_BonusChest no longer used). OnHackStateChanged and OnMarkedRocksChanged are not implemented.
- logic_in_bp: none (two empty events).
- assets: /Game/RogueRigs/Art/Chest/ SM_Chest_Body, SM_Chest_Lid, SM_Chest_Latch (imported from Art/Source/chest/clean/ at scale 1, no material or texture import, no collision set), T_Chest_BaseColor, M_Chest (TextureSampleParameter2D BaseColorTex to BaseColor, constants roughness 0.45, metallic 0). /Game/RogueRigs/Materials/GreyBox/MI_GB_BonusChest is no longer referenced by this Blueprint (left in place), nor is MI_GroundRingGold.
- owner_agent: agent-bp
- verified: task-306-03 (editor, no PIE): compiles without error with warnings_as_errors; components, parents, relative transforms, meshes and slot materials read back as above; look and lock-face direction not viewed on screen. task-056-02 PIE (alert speed cheat x40, console hack cheat): spawned as BP_BonusChest_C_0 at 60 % alert, no error in LogBlueprint, LogScript or LogRogueRigs. The chest hack completion and the OnRewardGranted glow were not exercised (no cheat completes the chest hack); look not viewed on screen.

## Relations
- [[blueprints-index]] — parent index.
- [[sys-objectives]] — the C++ class and events.
- [[bp-rr-game-mode]] — holds it in RunFlowConfig.
- [[sys-ground-telegraph]] — draws the hack zone circle.
- [[objective-bonus-chest]] — the design it implements.
