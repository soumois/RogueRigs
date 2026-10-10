---
id: task-040-locator-arrow
description: "Global task: Console locator: real arrow about 64 px, pulsing, with the distance in metres."
parent: tasks-done-index
kind: task
status: done
---
# Clearer console locator

- request: "La direction de la commande à pirater n'est pas clair" (owner, 2026-10-07)
- goal: Console locator: real arrow about 64 px, pulsing, with the distance in metres.
- acceptance: PIE check with no error; docs updated; lint clean; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: none
- result: Done. M_LocatorArrow (UI triangle tinted by vertex colour); WBP_HUD LocatorArrow is a 64 px Image rotated by ArrowAngle at EdgePosition, pulsing 1 + 0.15 sin(360 t); LocatorDistance text in metres 50 px below or above; teal console, white shuttle. PIE: arrow image and '36 m' shown with the console off screen; no runtime log error; pulse, rotation and shuttle colour not seen. Doc wbp-hud. Commits 1db00cc, b93ca58.

## Subtasks
1. task-040-01-locator-arrow — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-040-01-locator-arrow]] — step 1, bp.
