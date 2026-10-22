---
id: task-284-robot-walk-knee-anim
description: "Global task: Animate the heavy, shooter and chaser v2 walk with a light knee bend found on each mesh, and light kamikaze legs."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Robot walk animation with knees

- request: "et pour finir animer. Pour l'animation, essaye de comprendre où est l'articulation du genou pour améliorer l'animation de la marche (même si le mouvement de pliage sera léger comme ce sont des robots )" (owner, 2026-10-22)
- goal: Animate the heavy, shooter and chaser v2 walk with a light knee bend found on each mesh, and light kamikaze legs.
- acceptance: every subtask done and its checks passed; the four robots walk (kamikaze legs move very lightly) in a -game capture, knees bend lightly; committed.
- gdd_check: read enemy-heavy, enemy-kamikaze, enemy-shooter, enemy-chaser, chaser-meshy-pipeline, art-prompts-robots-meshy, the bp-* and sys-enemies-* nodes (2026-10-22). Owner decisions 2026-10-22 (provisional): Meshy triangle counts kept, no decimation (chaser v2 5120, heavy 10361, kamikaze 10172, shooter 7263; the chaser 3000 budget is lifted for v2); kamikaze keeps hover, only a very light leg motion, no walk; chaser v2 and a shield chaser v2 are tried in new assets, every chaser v1 and shield chaser v1 asset, file and Blueprint kept intact for restore; shooter gets walk, aim and fire clips. Defaults accepted by the owner (provisional, owner 2026-10-22): parts lists of heavy and kamikaze (spec open; default heavy, shooter, chaser v2: the 10 chaser parts; kamikaze: body and 2 lower legs); knee bend (default at most 15 degrees); spawner switched to BP_ChaserV2 (default yes, restore = switch back); conflict recorded: enemy-heavy and enemy-shooter said no animation in grey-box, the request decides walk clips.
- depends_on_global: task-283
- confirmed_by_owner: yes (2026-10-22, plan accepted; launch on the owner's word)
- milestone: M2
- next_step: task-284-05-cpp-anim-clips
- result: open

## Subtasks
1. task-284-01-parts-bake-heavy — 3d: Cut and bake the heavy walk
2. task-284-02-parts-bake-shooter — 3d: Cut and bake the shooter walk
3. task-284-03-parts-bake-chaser-v2 — 3d: Cut and bake the chaser v2 walk
4. task-284-04-parts-bake-kamikaze — 3d: Cut and bake the kamikaze leg sway
5. task-284-05-cpp-anim-clips — cpp: Shared anim clip writer for the robots
6. task-284-06-bp-heavy-anim — bp: BP_Heavy plays the walk
7. task-284-07-bp-shooter-anim — bp: BP_Shooter plays the walk
8. task-284-08-bp-chaser-v2-anim — bp: BP_ChaserV2 plays the run and attack
9. task-284-09-bp-kamikaze-anim — bp: BP_Kamikaze plays the leg sway
10. task-284-10-bp-spawner-chaser-v2 — bp: Spawner uses BP_ChaserV2
11. task-284-11-capture-walk — cpp: Capture the robots walking

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-284-01-parts-bake-heavy]] — step 01, 3d.
- [[task-284-02-parts-bake-shooter]] — step 02, 3d.
- [[task-284-03-parts-bake-chaser-v2]] — step 03, 3d.
- [[task-284-04-parts-bake-kamikaze]] — step 04, 3d.
- [[task-284-05-cpp-anim-clips]] — step 05, cpp.
- [[task-284-06-bp-heavy-anim]] — step 06, bp.
- [[task-284-07-bp-shooter-anim]] — step 07, bp.
- [[task-284-08-bp-chaser-v2-anim]] — step 08, bp.
- [[task-284-09-bp-kamikaze-anim]] — step 09, bp.
- [[task-284-10-bp-spawner-chaser-v2]] — step 10, bp.
- [[task-284-11-capture-walk]] — step 11, cpp.
