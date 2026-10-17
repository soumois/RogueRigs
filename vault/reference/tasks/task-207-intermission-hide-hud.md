---
id: task-207-intermission-hide-hud
description: "Global task: HUD and subtitles hidden during intermission and loading."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# HUD and subtitles hidden during intermission and loading

- request: "Oui, crée une tâche pour corriger les 3 défauts : sous-titres et HUD sous l'intermission, [...]" after the task-206-06 capture showed the Kessler-Voss subtitle over the Continuer button and HUD objective texts behind the title (owner, 2026-10-17)
- goal: the HUD and the current subtitle are hidden while the intermission or the loading screen is shown, and come back when the level starts.
- acceptance: a rendered capture of the intermission shows no HUD element and no subtitle; the HUD and subtitles work again on level 2; existing tests pass (2 known pre-existing failures excepted); docs updated; capture sent to the owner; lint clean; committed.
- gdd_check: [[intermission]] (gameplay paused). Owner 2026-10-17: hide the whole HUD (minimap, bars, weapons, objectives) and the current subtitle during intermission and loading, restore at level start.
- confirmed_by_owner: yes (2026-10-17)
- milestone: M2
- next_step: task-207-02-capture
- result: open

## Subtasks
1. task-207-01-hide-hud-code — cpp
2. task-207-02-capture — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-206-level-to-level]] — the transition this fixes.
- [[task-207-01-hide-hud-code]] — step 1, cpp.
- [[task-207-02-capture]] — step 2, cpp.
