---
id: task-317-stats-then-cards
description: "Global task: end-of-level stats screen and cards shop screen become two distinct screens; level 4 shows stats only, then run-won."
parent: tasks-done-index
kind: task
status: done
---
# Stats screen then cards screen

- request: "Aussi l'écran des stats de fin de niveau et l'écran des cartes sont deux écrans distincts. Je finis le niveau, je vois l'écran des stats de fin, je clique sur next level, j'arrive sur l'écran des cartes [...] Il n'y a pas la proposition d'écran des cartes pour lorsqu'on termine la run." Answers: Q1 "à la run 4, on affiche les stats, mais on affichera pas les cartes"; Q2 "Tu peux rajouter le nombre de robots morts par catégorie (mets juste les noms pour l'instant) et les dégâts par arme"; Q3 ok (cards screen on opaque black) (owner, 2026-10-26)
- goal: The end-of-level stats screen and the cards shop screen are two distinct screens; level 4 shows the stats screen only, then the run-won screen.
- acceptance: after a level 1-3 extraction the stats screen shows the level recap, ore this level and run, robots destroyed per robot type (names as text) and damage per weapon, with a Next level button; Next level opens a separate cards screen on an opaque black background with the 5 priced cards, balance, reroll and Continue; after the level 4 extraction the stats screen shows, then the run-won screen, no cards screen; -game captures of the three screens sent to the owner; docs and GDD updated; lint clean; committed.
- gdd_check: intermission, run-shop, run-structure, ore-economy read (2026-10-26). Conflicts: GDD had one recap+shop screen (owner: two screens, GDD updated); shop at run end not in the GDD (owner Q1: level 4 shows stats only, no cards). Owner Q2: add kills per robot type (names only, no icons; real counts from the run log KillsByRobotType) and damage per weapon (DamageDealtByWeapon). Q3 cards screen on opaque black: ok. Reading to confirm: on level 4 the stats button leads to the existing run-won screen. Old task-319 (run-end shop) dropped.
- confirmed_by_owner: yes (2026-10-26)
- milestone: M2
- next_step: none
- executor_tokens: input 40, cache write 351185, cache read 1440381, output 12918
- executor_usage: runs 1, turns 20, context start 55k peak 113k, resumes 0, 42 min, units 0.65 M
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Done. -game captures in Saved/StatsCards (contact-sheet.png): L1 stats (Chaser: 8, Burst rifle: 125, Next level), cards on black with 5 priced cards, L4 stats CLAIM EMPTIED / Cash out, then run-won screen, no cards on L4; no defect; unseen: mouse clicks, multi-entry lists, buy/reroll on new screen, nonzero ore. Commit f14a7110.

## Subtasks
1. task-317-01-stats-then-cards — cpp
2. task-317-02-text-stats-labels — text
3. task-317-03-bp-shop-screen — bp
4. task-317-04-bp-stats-screen — bp
5. task-317-05-capture-stats-cards — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-317-01-stats-then-cards]] — step 1, cpp.
- [[task-317-02-text-stats-labels]] — step 2, text.
- [[task-317-03-bp-shop-screen]] — step 3, bp.
- [[task-317-04-bp-stats-screen]] — step 4, bp.
- [[task-317-05-capture-stats-cards]] — step 5, cpp.
