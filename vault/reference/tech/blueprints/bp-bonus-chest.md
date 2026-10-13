---
id: bp-bonus-chest
description: "BP_BonusChest, Blueprint of ARRBonusChest: gold grey-box chest with a glow that grows with the hack and a glow burst on reward; open for its mesh, materials and events."
parent: blueprints-index
kind: doc
---
# BP_BonusChest

- asset_path: /Game/RogueRigs/Blueprints/Objectives/BP_BonusChest
- parent_class: ARRBonusChest (/Script/RogueRigs.RRBonusChest)
- purpose: the optional bonus chest dropped at 60 % alert by the run flow ([[sys-objectives]]); visuals only.
- components: duplicated from [[bp-objective-console]] then re-parented. no own component (the HackRing plane was removed by task-113-02; the zone circle is the inherited GroundTelegraph of [[sys-ground-telegraph]]); inherited Mesh: StaticMesh = engine Cube, scale (1.4, 1, 0.8), relative location z 40, material slot 0 = MI_GB_BonusChest.
- exposed_defaults: C++ defaults kept (hack 8 s, zone 6 cells, reward values). Set as `BonusChestClass` in `RunFlowConfig` of [[bp-rr-game-mode]] (compile BP_RRGameMode after editing it, else a PIE reads None).
- events: OnHackProgress(Progress, bHacking) as the console: SetScalarParameterValueOnMaterials(Mesh, GlowAmount, 0.15 + Progress * 3.5); the old SetRelativeScale3D(HackRing) call was removed by task-113-02. OnRewardGranted(XP, OreChunks, bUpgradeChoiceQueued) sets GlowAmount to 5 (glow burst). OnHackStateChanged and OnMarkedRocksChanged are not implemented.
- logic_in_bp: display only (two events).
- assets: /Game/RogueRigs/Materials/GreyBox/MI_GB_BonusChest (Color brown-gold 0.45/0.3/0.12, GlowColor gold 1/0.78/0.1). MI_GroundRingGold (/Game/RogueRigs/FX) is no longer used by this Blueprint and has no referencer; left in place.
- owner_agent: agent-bp
- verified: task-056-02 PIE (alert speed cheat x40, console hack cheat): spawned as BP_BonusChest_C_0 at 60 % alert, no error in LogBlueprint, LogScript or LogRogueRigs. The chest hack completion and the OnRewardGranted glow were not exercised (no cheat completes the chest hack); look not viewed on screen.

## Relations
- [[blueprints-index]] — parent index.
- [[sys-objectives]] — the C++ class and events.
- [[bp-rr-game-mode]] — holds it in RunFlowConfig.
- [[sys-ground-telegraph]] — draws the hack zone circle.
- [[objective-bonus-chest]] — the design it implements.
