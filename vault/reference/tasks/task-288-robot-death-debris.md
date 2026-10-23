---
id: task-288-robot-death-debris
description: "Global task: Death debris pieces for heavy, kamikaze, shooter, chaser v2 and shield chaser v2, as for the chaser, v1 debris untouched."
parent: tasks-done-index
kind: task
status: done
---
# Robot death debris for the new meshes

- request: "Pour le 9, prépare les débris aussi." (owner, 2026-10-22; point 9 = death debris pieces for the new robots)
- goal: Death debris pieces for heavy, kamikaze, shooter, chaser v2 and shield chaser v2, as for the chaser, v1 debris untouched.
- acceptance: every subtask done and its checks passed; each new robot throws 2 to 4 of its own pieces (kamikaze 2 or 3) in a -game capture; v1 debris assets unchanged; committed.
- gdd_check: read enemy-death, bp-robot-debris, chaser-meshy-pipeline step 6, sys-enemies-base (2026-10-22). Kamikaze parts are body and 2 lower legs (owner 2026-10-22), so it has no arm and no knee cut: 2 or 3 pieces. Shield chaser v2 reuses the chaser v2 pieces and the v1 shield shards (same shield mesh).
- depends_on_global: task-284 (parts meshes), task-286 (shield chaser v2, step 15 only)
- confirmed_by_owner: yes (2026-10-22, plan accepted; launch on the owner's word)
- milestone: M2
- next_step: none
- result: Done. Death shots Saved/RobotMeshes/debris/ (sheets/ contact sheets) via a scratch -game script (SpawnNear 450 yaw -30, crit 1, slomo 0.25, timed Shots); heavy 3, shooter 3, chaser v2 3, kamikaze 3 (Niagara dump NumParticles 3, reads as 5-6 objects on screen), shield chaser v2 >=3 v2 pieces + v1 shards (not separable); no glow; all gone by 4.9-6.1 s after death; chaser v1 reference 2 small pieces. Unverified: piece identities, exact counts except kamikaze.

## Subtasks
1. task-288-01-pieces-heavy — 3d: Debris pieces of the heavy
2. task-288-02-pieces-shooter — 3d: Debris pieces of the shooter
3. task-288-03-pieces-chaser-v2 — 3d: Debris pieces of the chaser v2
4. task-288-04-pieces-kamikaze — 3d: Debris pieces of the kamikaze
5. task-288-05-cpp-debris-groups — cpp: Debris launch for robots with fewer parts
6. task-288-06-bp-debris-material — bp: Robot debris material and instances
7. task-288-07-bp-ns-heavy-debris — bp: NS_HeavyDebris with its pieces
8. task-288-08-bp-ns-shooter-debris — bp: NS_ShooterDebris with its pieces
9. task-288-09-bp-ns-chaser-v2-debris — bp: NS_ChaserV2Debris with its pieces
10. task-288-10-bp-ns-kamikaze-debris — bp: NS_KamikazeDebris with its pieces
11. task-288-11-bp-heavy-death — bp: BP_Heavy throws its debris
12. task-288-12-bp-shooter-death — bp: BP_Shooter throws its debris
13. task-288-13-bp-chaser-v2-death — bp: BP_ChaserV2 throws its debris
14. task-288-14-bp-kamikaze-death — bp: BP_Kamikaze throws its debris
15. task-288-15-bp-shield-v2-death — bp: BP_ShieldChaserV2 throws chaser v2 debris
16. task-288-16-capture-debris — cpp: Capture the new robot deaths

## Relations
- [[tasks-done-index]] — parent index.
- [[task-288-01-pieces-heavy]] — step 01, 3d.
- [[task-288-02-pieces-shooter]] — step 02, 3d.
- [[task-288-03-pieces-chaser-v2]] — step 03, 3d.
- [[task-288-04-pieces-kamikaze]] — step 04, 3d.
- [[task-288-05-cpp-debris-groups]] — step 05, cpp.
- [[task-288-06-bp-debris-material]] — step 06, bp.
- [[task-288-07-bp-ns-heavy-debris]] — step 07, bp.
- [[task-288-08-bp-ns-shooter-debris]] — step 08, bp.
- [[task-288-09-bp-ns-chaser-v2-debris]] — step 09, bp.
- [[task-288-10-bp-ns-kamikaze-debris]] — step 10, bp.
- [[task-288-11-bp-heavy-death]] — step 11, bp.
- [[task-288-12-bp-shooter-death]] — step 12, bp.
- [[task-288-13-bp-chaser-v2-death]] — step 13, bp.
- [[task-288-14-bp-kamikaze-death]] — step 14, bp.
- [[task-288-15-bp-shield-v2-death]] — step 15, bp.
- [[task-288-16-capture-debris]] — step 16, cpp.
