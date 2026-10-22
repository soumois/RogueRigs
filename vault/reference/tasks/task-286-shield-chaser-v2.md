---
id: task-286-shield-chaser-v2
description: "Global task: Build a shield chaser on the chaser v2 mesh in new assets (BP_ShieldChaserV2), the v1 shield chaser kept intact for restore."
parent: tasks-done-index
kind: task
status: done
---
# Shield chaser v2

- request: "Chasseur: essayer le chasseur v2 ET un chasseur v2 avec bouclier; garder la version v1 (assets, fichiers, BP) intacte pour pouvoir la restaurer (owner answer relayed by the main session)" (owner, 2026-10-22)
- goal: Build a shield chaser on the chaser v2 mesh in new assets (BP_ShieldChaserV2), the v1 shield chaser kept intact for restore.
- acceptance: every subtask done and its checks passed; BP_ShieldChaserV2 spawns in game with the shield run and bash clips, v1 assets unchanged; committed.
- gdd_check: read enemy-heavy, enemy-kamikaze, enemy-shooter, enemy-chaser, chaser-meshy-pipeline, art-prompts-robots-meshy, the bp-* and sys-enemies-* nodes (2026-10-22). Owner decisions 2026-10-22 (provisional): Meshy triangle counts kept, no decimation (chaser v2 5120, heavy 10361, kamikaze 10172, shooter 7263; the chaser 3000 budget is lifted for v2); kamikaze keeps hover, only a very light leg motion, no walk; chaser v2 and a shield chaser v2 are tried in new assets, every chaser v1 and shield chaser v1 asset, file and Blueprint kept intact for restore; shooter gets walk, aim and fire clips. Defaults accepted by the owner (provisional, owner 2026-10-22): spawner switched to BP_ShieldChaserV2 (default yes, restore = switch back); death debris of the v2 robots in task-288 (owner 2026-10-22).
- depends_on_global: task-284
- confirmed_by_owner: yes (2026-10-22, plan accepted; launch on the owner's word)
- milestone: M2
- next_step: none
- result: Done. Shield chaser v2 = chaser v2 parts + v1 riot shield on the left forearm (grip lowered 16->4 cm), four v1 clips at RunRate 1.5 (286-01); BP_ShieldChaserV2 with new M_ShieldChaserV2_Anim and instances, v1 untouched (286-02); spawner fifth entry on BP_ShieldChaserV2 (286-03); shots Saved/RobotMeshes/shield-v2/. Unverified: eyes on camera.

## Subtasks
1. task-286-01-shield-fit-bake — 3d: Fit the shield on chaser v2 and bake
2. task-286-02-bp-shield-chaser-v2 — bp: BP_ShieldChaserV2 with its materials
3. task-286-03-bp-spawner-shield-v2 — bp: Spawner uses BP_ShieldChaserV2
4. task-286-04-capture-shield-v2 — cpp: Capture the shield chaser v2

## Relations
- [[tasks-done-index]] — parent index.
- [[task-286-01-shield-fit-bake]] — step 01, 3d.
- [[task-286-02-bp-shield-chaser-v2]] — step 02, bp.
- [[task-286-03-bp-spawner-shield-v2]] — step 03, bp.
- [[task-286-04-capture-shield-v2]] — step 04, cpp.
