---
id: task-283-robot-mesh-integration
description: "Global task: Integrate the flattened, cleaned heavy, kamikaze and shooter meshes into their Blueprints, and prepare chaser v2, replacing the grey-box primitives."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Robot meshes: integrate into Unreal

- request: "Ensuite, il faudra intégrer les mesh" (owner, 2026-10-22)
- goal: Integrate the flattened, cleaned heavy, kamikaze and shooter meshes into their Blueprints, and prepare chaser v2, replacing the grey-box primitives.
- acceptance: every subtask done and its checks passed; BP_Heavy, BP_Kamikaze and BP_Shooter show their Meshy mesh in a -game capture compared with the grey-box capture; committed.
- gdd_check: read enemy-heavy, enemy-kamikaze, enemy-shooter, enemy-chaser, chaser-meshy-pipeline, art-prompts-robots-meshy, the bp-* and sys-enemies-* nodes (2026-10-22). Owner decisions 2026-10-22 (provisional): Meshy triangle counts kept, no decimation (chaser v2 5120, heavy 10361, kamikaze 10172, shooter 7263; the chaser 3000 budget is lifted for v2); kamikaze keeps hover, only a very light leg motion, no walk; chaser v2 and a shield chaser v2 are tried in new assets, every chaser v1 and shield chaser v1 asset, file and Blueprint kept intact for restore; shooter gets walk, aim and fire clips. Defaults accepted by the owner (provisional, owner 2026-10-22): game heights (default: chaser v2 123 cm as v1, heavy 248, shooter 176, kamikaze 194 = current capsules); heavy telegraph glow on the new mesh (default: eye slot glows, stripes no longer glow).
- depends_on_global: task-282
- confirmed_by_owner: yes (2026-10-22, plan accepted; launch on the owner's word)
- milestone: M2
- next_step: task-283-03-game-ready-kamikaze
- result: open

## Subtasks
1. task-283-01-capture-before — cpp: Capture the robots before integration
2. task-283-02-game-ready-heavy — 3d: Make the heavy mesh game ready
3. task-283-03-game-ready-kamikaze — 3d: Make the kamikaze mesh game ready
4. task-283-04-game-ready-shooter — 3d: Make the shooter mesh game ready
5. task-283-05-game-ready-chaser-v2 — 3d: Make the chaser v2 mesh game ready
6. task-283-06-robot-body-material — bp: Robot body material and instances
7. task-283-07-bp-heavy-mesh — bp: BP_Heavy uses the Meshy mesh
8. task-283-08-bp-kamikaze-mesh — bp: BP_Kamikaze uses the Meshy mesh
9. task-283-09-bp-shooter-mesh — bp: BP_Shooter uses the Meshy mesh
10. task-283-10-capture-after — cpp: Capture the robots after integration

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-283-01-capture-before]] — step 01, cpp.
- [[task-283-02-game-ready-heavy]] — step 02, 3d.
- [[task-283-03-game-ready-kamikaze]] — step 03, 3d.
- [[task-283-04-game-ready-shooter]] — step 04, 3d.
- [[task-283-05-game-ready-chaser-v2]] — step 05, 3d.
- [[task-283-06-robot-body-material]] — step 06, bp.
- [[task-283-07-bp-heavy-mesh]] — step 07, bp.
- [[task-283-08-bp-kamikaze-mesh]] — step 08, bp.
- [[task-283-09-bp-shooter-mesh]] — step 09, bp.
- [[task-283-10-capture-after]] — step 10, cpp.
