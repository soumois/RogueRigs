---
id: task-055-first-hit-vignette
description: "Global task: The red damage vignette shows on the very first hit, including in a packaged build."
parent: tasks-index
kind: task
status: in-progress
---
# First-hit vignette

- request: "La vignette rouge absente au premier coup, à vérifier dans un build packagé — peut-être un souci d’initialisation" (owner, 2026-10-07)
- goal: The red damage vignette shows on the very first hit, including in a packaged build.
- acceptance: build exits 0 with the editor closed; packaged build shows the vignette on the first hit (described shot); all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: task-055-01-first-hit-vignette
- result: open

## Subtasks
1. task-055-01-first-hit-vignette — cpp

## Relations
- [[tasks-index]] — parent index.
- [[task-055-01-first-hit-vignette]] — step 1, cpp.
