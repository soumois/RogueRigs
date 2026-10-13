---
id: task-106-weapons-ui-size
description: "Global task: the weapon slots are bigger, like the DRGS reference, and the columns have their set width. Prepared, not launched."
parent: tasks-done-index
kind: task
status: done
---
# Weapons UI size

- request: "l'interface des armes est devenue trop petite (elle est bien centrée); je voudrais la taille de l'exemple DRGS, centre bas" (owner playtest 2026-10-12, prepared only, not launched) (owner, 2026-10-12)
- goal: The weapon slots are bigger, like the DRGS reference (framed slots at the bottom centre), and the columns really have their set width.
- acceptance: cause of the collapsed columns found and fixed; slot sizes read back from the asset match the values above (or the owner values); no overlap computed with subtitles, turret line and XP bar (1920 x 1080 reference); compiled 0 errors; saved; docs updated; lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-12).
- confirmed_by_owner: yes (2026-10-12): the proposed sizes are OK for now; the owner will come back later for better positions
- milestone: M2
- next_step: none
- result: Done. Four icons imported (TextureTools.import_file, UI group, no mips, sRGB) and shown in the weapon slots; GetWeaponIcon(Name) single mapping; padlock for LOCKED slots; cell 150 x 90 with icon 42 x 42, name 16 pt, bar 130 x 14; not seen on screen. Commits c046e63, aa51dc7, 6271733.

## Subtasks
1. task-106-01-weapons-ui-size — bp
2. task-106-02-weapons-ui-size — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-106-01-weapons-ui-size]] — step 1, bp.
- [[task-106-02-weapons-ui-size]] — step 2, bp.
