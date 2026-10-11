---
id: task-094-hud-refactor
description: "Global task: the HUD is split into one widget per logic, placed by WBP_HUD, and driven by events instead of ticks."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# HUD refactor: audit

- request: "refonte de l'interface (094)": un widget par logique, WBP_HUD seulement racine qui place, des événements au lieu de vérifier à chaque tick (owner 2026-10-11) (owner, 2026-10-11)
- goal: The HUD is split into one widget per logic, all placed by WBP_HUD, which only lays them out; widgets react to events, tick only for continuous values.
- acceptance: the audit note exists with the table per feature, the proposed sub-widget list and the missing-events list with payloads; no asset changed (git status shows no uasset change); lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-11).
- confirmed_by_owner: yes (2026-10-11)
- milestone: M2
- next_step: task-094-03-hud-health
- result: open

## Subtasks
1. task-094-01-hud-refactor — bp
2. task-094-02-hud-events — cpp
3. task-094-03-hud-health — bp
4. task-094-04-hud-xp — bp
5. task-094-05-hud-weapons — bp
6. task-094-06-hud-cards — bp
7. task-094-07-hud-alerts — bp
8. task-094-08-hud-minimap — bp
9. task-094-09-hud-root — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-094-01-hud-refactor]] — step 1, bp.
- [[task-094-02-hud-events]] — step 2, cpp.
- [[task-094-03-hud-health]] — step 3, bp.
- [[task-094-04-hud-xp]] — step 4, bp.
- [[task-094-05-hud-weapons]] — step 5, bp.
- [[task-094-06-hud-cards]] — step 6, bp.
- [[task-094-07-hud-alerts]] — step 7, bp.
- [[task-094-08-hud-minimap]] — step 8, bp.
- [[task-094-09-hud-root]] — step 9, bp.
