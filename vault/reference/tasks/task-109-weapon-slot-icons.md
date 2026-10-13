---
id: task-109-weapon-slot-icons
description: "Global task: Active weapon slots show the weapon icon large without its name, like the reference game; the icon tool keeps the icon aspect ratio."
parent: tasks-done-index
kind: task
status: done
---
# Weapon icon larger, no name on active slots

- request: "rapport de test du 2026-10-12" (owner); decisions confirmed 2026-10-12 (rules of the zones unchanged: only the rendering changes) (owner, 2026-10-13)
- goal: Active weapon slots show the weapon icon large without its name, like the reference game; the icon tool keeps the icon aspect ratio.
- acceptance: three trimmed weapon icons with their aspect ratio kept, checked by script; tool option documented; lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: none
- result: Done. Weapon icons reimported with their aspect ratio (delete and import, UI group re-set, GetWeaponIcon pins re-set); owned slot: name collapsed (fallback only), icon in a max 130 x 56 SizeBox ScaleToFit above the 130 x 14 bar; padlock unchanged; not seen on screen. Commits b90b7d0, fe20a80, d049954.

## Subtasks
1. task-109-01-weapon-slot-icons — cpp
2. task-109-02-weapon-slot-icons — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-109-01-weapon-slot-icons]] — step 1, cpp.
- [[task-109-02-weapon-slot-icons]] — step 2, bp.
