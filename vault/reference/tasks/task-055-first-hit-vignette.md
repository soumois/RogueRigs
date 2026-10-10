---
id: task-055-first-hit-vignette
description: "Global task: The red damage vignette shows on the very first hit, including in a packaged build."
parent: tasks-done-index
kind: task
status: done
---
# First-hit vignette

- request: "La vignette rouge absente au premier coup, à vérifier dans un build packagé — peut-être un souci d’initialisation" (owner, 2026-10-07)
- goal: The red damage vignette shows on the very first hit, including in a packaged build.
- acceptance: build exits 0 with the editor closed; packaged build shows the vignette on the first hit (described shot); all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: none
- result: Done. WBP_HUD UpdateVignette: HitHoldTime 0.15 s (editable) holds the flash at full value after a new hit before the 2 per second decay; hold decremented after the flash is applied so a long frame still shows it; a lower new hit during a decay is not seen as new (after healing only). With task-055-01 (warm opacity floor, packaged vignette on the first hit at normal frame rate). PIE: no log error. Commit a10d0f2.

## Subtasks
1. task-055-01-first-hit-vignette — cpp
2. task-055-02-bp-flash-hold — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-055-01-first-hit-vignette]] — step 1, cpp.
- [[task-055-02-bp-flash-hold]] — step 2, bp.
