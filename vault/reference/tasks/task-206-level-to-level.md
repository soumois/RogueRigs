---
id: task-206-level-to-level
description: "Global task: a run chains 4 levels with an intermission recap, background layout loading, a loading screen; PIE starts directly."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Level to level: intermission and loading

- request: "On va travailler le fait de passer de niveau en niveau. [...] Plus tard, il y aura surement un écran assez simple pour acheter des améliorations avec les ressources qu'on aura acheté. Prévoit la possibilité d'avoir ce moment là et pendant que le joueur se décide pour acheter les améliorations, on commencera déjà à génerer le terrain. Si la géneration de terrain n'est pas encore fini, on arrivera sur un écran de chargement. ( pour l'instant, le chargement ne servira qu'à charger la map et préchauffer la pool ) On doit toujours pouvoir lancer le jeu depuis l'éditeur en étant dans un niveau sans devoir passer par un menu." Then: "L'écran intermédiaire est intéressant pour le placeholder et permet de faire un recap" (owner, 2026-10-16 and 2026-10-17)
- goal: a run chains 4 levels: after each extraction an intermission recap screen, the next layout prepared in the background, a loading screen if not ready; PIE still starts directly.
- acceptance: in a -game run, a successful extraction on level 1 shows the intermission (level recap: ore secured this level and run total; empty shop area; Continue) while the level 2 layout loads in the background; Continue shows the loading screen until the map is loaded, the grid built and the pool prewarmed, then level 2 starts on arena_l2 with cards, weapons, level, XP, health and ore carried over; level 4 extraction shows the run-won screen; a loss shows run-lost and restart starts level 1; Play in Editor on L_ArenaM1 starts level 1 with no menu; captures sent to the owner; existing tests pass; docs updated; lint clean; committed.
- gdd_check: [[run-structure]], [[intermission]], [[run-shop]], [[ore-economy]], [[extraction]], [[level-flow]]. Conflicts resolved by the owner 2026-10-17: run-structure had "nothing in between" (now intermission); background generation means the future C++ generator (no runtime AI), until then a pre-written layout loaded asynchronously; in-run shop paid in collected ore (also rerolls later), content open, not built; recap later shows DPS per weapon and robots killed once run logs exist; carried: cards, weapons, level, XP, health, ore; console on every level; no difficulty ramp yet (future balance task); loss restarts at level 1; level 1 stays arena_m1_fine.
- confirmed_by_owner: yes (2026-10-17)
- milestone: M2
- next_step: task-206-03-transition-flow
- result: open

## Subtasks
1. task-206-01-run-state — cpp
2. task-206-02-async-layout — cpp
3. task-206-03-transition-flow — cpp
4. task-206-04-wbp-intermission — bp
5. task-206-05-wbp-loading — bp
6. task-206-06-transition-capture — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-206-01-run-state]] — step 1, cpp.
- [[task-206-02-async-layout]] — step 2, cpp.
- [[task-206-03-transition-flow]] — step 3, cpp.
- [[task-206-04-wbp-intermission]] — step 4, bp.
- [[task-206-05-wbp-loading]] — step 5, bp.
- [[task-206-06-transition-capture]] — step 6, cpp.
