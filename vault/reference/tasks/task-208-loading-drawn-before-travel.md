---
id: task-208-loading-drawn-before-travel
description: "Global task: Loading screen drawn before the map travel."
parent: tasks-done-index
kind: task
status: done
---
# Loading screen drawn before the map travel

- request: "Oui, crée une tâche pour corriger les 3 défauts : [...] écran de chargement de l'ancienne map jamais affiché, [...]" (owner, 2026-10-17)
- goal: after Continue, the loading screen is drawn at least once on the old map before the map travel starts, so the frozen game is never shown.
- acceptance: a rendered capture of the first frame after Continue shows the loading screen; the log shows at least one rendered frame between loading started and the travel; tests pass (2 known failures excepted); docs updated; capture sent to the owner; lint clean; committed.
- gdd_check: [[intermission]] (loading screen covers the map load). Owner 2026-10-17: delay the travel until the loading widget has been drawn at least once (1 to 2 frames).
- confirmed_by_owner: yes (2026-10-17)
- milestone: M2
- next_step: none
- result: Done. Rendered -game 1920 x 1080 captures sent to the owner: frames 1946 and 1947 on the old map show the loading screen (DROPPING IN, Level 2, Loading the map) before the blocking load, then the new-world loading screen and level 2 with the HUD. Observations: LoadMap's own redraw after BeginTearingDown captured the old scene without UMG (probably not presented, unproven); dev-only Preparing Shaders messages over the first new-world frames. Saved/Captures/task208_*.png.

## Subtasks
1. task-208-01-delay-travel-code — cpp
2. task-208-02-capture — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-206-level-to-level]] — the transition this fixes.
- [[task-208-01-delay-travel-code]] — step 1, cpp.
- [[task-208-02-capture]] — step 2, cpp.
