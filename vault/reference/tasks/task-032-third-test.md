---
id: task-032-third-test
description: "Global task: third play test: robot recycling, console locator, overtime, slower robots, faster heavy, English texts, upgrade weights, weapon cards every 5 levels, bigger turret."
parent: tasks-index
kind: task
status: in-progress
---
# Third play test fixes

- request: "le level est toujours trop lent actuellement. Aussi, à 2-3 min de jeu, je me suis mis à courrir sans cesse et je n'avais pas de monstre qui popait en face de moi. Il faut aussi un indicateur de où la console de commande. [...] j'ai pu jouer genre 5-6 min (en étant coincé à la moitié) alors qu'un niveau doit faire 3-4 min. Le niveau de difficulté aurait du augmenter plus vite à un certains moment pour m'empecher de rester dans le niveau indéfiniment. Réduit encore la vitesse de 10% et accélére un peu l'attaque du gros. Aussi met les texte en anglais pour l'instant. Aussi je pense que certaines capacités ne doivent apparaitre plus souvent (damage, fire rate, reload) que d'autres. Aussi, les une ou deux armes sont proposées tout les 5 niveaux. Grossi de 50% le rendu de la tourelle, on ne la voit presque pas." (2026-10-06; plan accepted with "ok")
- goal: running never starves spawns ahead; the console is easy to find; a level cannot last much past 4 min; robots slower; heavy attacks faster; English UI; damage, fire rate and reload offered more often; weapons offered every 5 levels; visible turret.
- acceptance: (1) build exits 0; all tests pass; (2) robots off screen > 2500 uu for 4 s are removed and new spawns appear ahead of a running miner after 3 min (test); (3) overtime from 3:30: horde every 30 s, wave interval halves per overtime minute (test); (4) speeds chaser and shield chaser 182, kamikaze and shooter 151, heavy 101; heavy telegraph 0.6 s, slam every 2 s; (5) upgrade weights 3 for damage, fire rate, reload, 1 otherwise; weapon cards only on levels 5, 10, 15… (1, or 2 when 2+ remain) (tests); (6) every on-screen text in English; (7) edge arrow to the console, then the shuttle; turret drawn 50 % larger (PIE); (8) docs; lint clean; committed. Then task-031-03 retunes XP pacing on the result.
- gdd_check: Read [[enemies-index]], enemy nodes, [[difficulty]], [[upgrades]], [[turret]], [[objective-console]], [[readability-rules]]. GDD updated by the Orchestrator with provisional values (2026-10-06). No open value.
- confirmed_by_owner: yes (2026-10-06)
- milestone: M2
- next_step: task-032-01-cpp-fixes
- result: open

## Subtasks
1. task-032-01-cpp-fixes — cpp: recycling, overtime, speeds, heavy timing, upgrade weights, weapon cards every 5 levels, English C++ texts, locator target getter, tests
2. task-032-02-bp-fixes — bp: edge locator arrow, English Blueprint texts, turret 50 % larger, PIE

## Relations
- [[tasks-index]] — parent index.
- [[task-032-01-cpp-fixes]] — step 1, cpp.
- [[task-032-02-bp-fixes]] — step 2, bp.
- [[task-031-directional-spawn]] — its XP retune (031-03) runs after this task.
