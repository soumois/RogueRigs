---
id: task-103-hud-events-only
description: "Global task: the HUD parts that still poll every frame react to events; only continuous values keep a per-frame update."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# HUD per-frame polling to events: C++ events

- request: "connecte à des événements plutôt que check chaque frame pour essayer d'optimiser" (owner 2026-10-11, after the HUD refactor): only continuous values (for example the reload progress) check every frame, everything else reacts to events (owner, 2026-10-12)
- goal: The HUD parts that still poll every frame (minimap redraw and texture, locator arrow, vignette warm-up) react to events instead; only genuinely continuous values keep a per-frame update.
- acceptance: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; NativeTick holds no per-frame polling except a justified continuous value; docs updated; lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-12).
- confirmed_by_owner: yes (2026-10-12)
- milestone: M2
- next_step: task-103-01-hud-events-only
- result: open

## Subtasks
1. task-103-01-hud-events-only — cpp
2. task-103-02-hud-events-only — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-103-01-hud-events-only]] — step 1, cpp.
- [[task-103-02-hud-events-only]] — step 2, bp.
