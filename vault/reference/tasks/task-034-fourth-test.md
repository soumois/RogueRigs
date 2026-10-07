---
id: task-034-fourth-test
description: "Global task: fourth play test: bigger projectile hitbox, whole-vein mining, heal pickup fix, small terrain capsules, kamikaze trigger, softer early waves, crowd, ammo HUD."
parent: tasks-index
kind: task
status: in-progress
---
# Fourth play test fixes

- request: "Augmenter encore la collision du projectile de 20%. Aussi quand on tape un bloc de minerai (composé de 9 blocs actuellement), on doit miner la totalité en même temps. Je n'ai pas réussi à ramasser de la vie [...]. Le gros devrait avoir la même collision pour le terrain que le chasseur (et les autres robot aussi) (pour le terrain seulement). [...] je dois vraiment être très proche d'un kamikaze pour le trigger [...] il s'arrête lorsqu'il est trigger, ça doit être le cas. Un peu trop de kamikaze et de tireur aussi. Le nombre d'ennemi monte un peu trop vite dans les bas niveau [...]. Lorsque des ennemis sont trop loin derrière, il faudrait peut être les retirer [...]. Les robots doivent moins se gêner entre eux aussi. Augmente le temps de préparation d'explosion [...] de 0.15 sec." Then: "double le temps de rechargement de la tourelle", ammo bars and blocks per weapon with a red reload bar and the turret recharge block rule, "Indique juste des slots locké pour l'instant" (2026-10-06; plan accepted)
- goal: the owner's fourth play test issues fixed.
- acceptance: (1) build exits 0; existing tests updated, all pass; (2) player projectile sphere 14 uu; (3) a touched 3 × 3 ore or heal vein breaks whole after 1.5 s with 9 chunks; (4) heal pickups always collectable, even when spawned inside the pickup radius; (5) every robot uses the chaser capsule against rock; robots do not block each other, light separation; (6) kamikaze arms at 250 uu, stops, explodes 0.65 s later; (7) packs 1–2 kamikazes, shooters at most 1 in one pack out of two, none before 1:30, at most 4 alive; packs 6–10 robots until 1:30; (8) recycling once off screen and > 1800 uu for 3 s; (9) turret recharge 12 s; (10) HUD weapon slots: ammo bar above 7 rounds, blocks at 7 or fewer, red reload bar, turret charge blocks with the recharge block in the leftmost empty block, padlock on slots not owned; (11) docs; lint clean; committed.
- gdd_check: GDD updated by the Orchestrator with provisional values (2026-10-06): burst-rifle, ore-blocks, heal-ore, enemy-kamikaze, enemies-index, turret, readability-rules. No open value.
- confirmed_by_owner: yes (2026-10-06)
- milestone: M2
- next_step: task-034-03-docs
- result: open

## Subtasks
1. task-034-01-cpp-fixes — cpp: points 1 to 6, 8, 9 (stopped there, split ordered by the owner)
2. task-034-03-docs — cpp: docs for 034-01, full run
3. task-034-04-director-mix — cpp: point 7, softer early waves
4. task-034-05-hud-ammo-data — cpp: point 10 data
5. task-034-02-bp-ammo-hud — bp: weapon slot ammo HUD, padlocks; PIE

## Relations
- [[tasks-index]] — parent index.
- [[task-034-01-cpp-fixes]] — step 1, cpp.
- [[task-034-03-docs]] — step 2, cpp.
- [[task-034-04-director-mix]] — step 3, cpp.
- [[task-034-05-hud-ammo-data]] — step 4, cpp.
- [[task-034-02-bp-ammo-hud]] — step 5, bp.
