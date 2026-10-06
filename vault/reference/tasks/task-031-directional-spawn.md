---
id: task-031-directional-spawn
description: "Global task: second play test: target dummy removed, closer spawns biased toward the run heading, robots 30 % slower, steadfast shooters, faster early levels."
parent: tasks-index
kind: task
status: done
---
# Directional spawn and second pacing pass

- request: "Tu peux clean les ennemis qui ne bougent pas aussi (le code, j'ai déjà clean dans le level) --- TargetDummy. En restant sans bouger, il faut quand même 6 sec pour qu'un enemi arrive. En courrant, j'ai réussi à ne pas voir d'ennemi pendant 20 sec. Aussi, il faut prendre en compte la direction de course du joueur pour le spawn. [...] Les premiers niveaux sont beaucoup trop long à monter. Les enemis sont encore trop rapide, je n'essaye jamais de miner car je n'ai jamais le temps. [...] réduit leur vitesse de 30%. [...] si un joueur va dans une direction (exemple nord), une partie du spawn doit se trouver au nord. ex: Si la direction est nord, 55% entre NE et NO, 35% entre NE et SE / NO et SE, 10% sud (à équilibrer). [...] je m'approche d'un tireur, il ne doit pas chercher à s'éloigner." (2026-10-06; plan accepted with "ok")
- goal: no target dummy left; first contact within about 2 s; running in one direction meets robots ahead; robots 30 % slower; shooters hold their ground; early levels come much faster.
- acceptance: (1) build exits 0; all tests pass; (2) no ARRTargetDummy code, BP_TargetDummy and MI_GB_Dummy deleted if unreferenced, docs removed or updated; (3) spawn band 100–500 uu beyond the view edge; directional split 55 / 35 / 10 measured in a test while running, uniform when still; packs and horde side follow it; (4) speeds chaser and shield chaser 202, kamikaze 168, shooter 168, heavy 112; (5) shooter never moves away from the miner (test); (6) Collect bot profile medians at L2 0:15, L5 1:10, L10 3:00 ± 20 % (owner 2026-10-06: the run profile is slower by design, reported only); (7) docs; lint clean; committed.
- gdd_check: Read [[enemies-index]], enemy nodes, [[xp-level-up]]. HP already match the owner's wish (chaser 3 rifle bullets, shield chaser 6): unchanged, reported to the owner. GDD updated by the Orchestrator with provisional values (2026-10-06). No open value.
- confirmed_by_owner: yes (2026-10-06)
- milestone: M2
- next_step: none
- result: Done. Target dummy removed; spawn band 100–500 uu with heading sectors 55/35/10 and detour filter; robots 30 % slower; shooters never retreat; XP curve level 1 = 2 then level² (owner). Running without collecting levels slower by design (owner).

## Subtasks
1. task-031-01-remove-dummy — cpp: remove the target dummy code, assets and docs; tests use a still chaser
2. task-031-02-spawn-speed-shooter — cpp: closer spawn band, heading split, speeds −30 %, shooter never retreats, tests
3. task-031-03-pacing-retune — cpp: two bot profiles, XP curve to the new targets, shots running north

## Relations
- [[tasks-index]] — parent index.
- [[task-031-01-remove-dummy]] — step 1, cpp.
- [[task-031-02-spawn-speed-shooter]] — step 2, cpp.
- [[task-031-03-pacing-retune]] — step 3, cpp; runs after task-032.
- [[task-032-third-test]] — owner third test, runs before 031-03.
