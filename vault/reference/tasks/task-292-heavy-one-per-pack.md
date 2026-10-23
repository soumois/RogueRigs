---
id: task-292-heavy-one-per-pack
description: "Global task: Packs carry at most 1 heavy so heavies arrive from different sides; heavy alive cap 3 unchanged."
parent: tasks-done-index
kind: task
status: done
---
# At most one heavy per pack

- request: "Le gros robot, je les vois souvent arriver par 3 et pas par différents groupes" (owner, 2026-10-23)
- goal: Heavies and WideHeavies pack types hold 1 heavy (was 2..3) at the front; in hordes the 2..3 heavies are spread along the front row instead of side by side; pack weights, alive cap 3 and every other director value unchanged (provisional, owner 2026-10-23).
- acceptance: no pack holds more than 1 heavy; horde heavies spread along the front; existing Spawner.Director tests updated and passing; sys-spawner-director-defaults updated; committed.
- gdd_check: enemies-index and enemy-heavy read (2026-10-23): rule added (at most 1 heavy per pack, provisional, owner 2026-10-23); alive cap 3 kept. Defaults accepted (provisional, owner 2026-10-23, "ok pour les défauts"): pack weights 96/4 kept (fewer heavies per run), horde heavies 2..3 kept but spread along the front, MakeLastLevel gets the same rule, alive cap 3 unchanged.
- confirmed_by_owner: yes (2026-10-23, "Le reste est bon"; planned in a planning pass, each task run later by its own orchestrator)
- milestone: M2
- next_step: none
- result: Done. Heavies and WideHeavies packs hold 1 heavy (was 2..3) in MakeDefault and MakeLastLevel; horde heavies spread along the front row (PrepareHorde, ends and middle; test cells (96,138) (109,138) (134,138)); weights 96/4 and alive cap 3 unchanged; build clean; RogueRigs.Spawner 20/20 pass; DirectorHorde gained a non-adjacency check; not seen in game.

## Subtasks
1. task-292-01-cpp-heavy-one-per-pack — cpp: Director: one heavy per pack

## Relations
- [[tasks-done-index]] — parent index.
- [[task-292-01-cpp-heavy-one-per-pack]] — step 01, cpp.
