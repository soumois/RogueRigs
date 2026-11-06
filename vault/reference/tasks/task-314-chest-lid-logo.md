---
id: task-314-chest-lid-logo
description: "Global task: Add the yellow Kessler-Voss mark on top of the chest lid, as on the validated sheet, and show it in game."
parent: tasks-done-index
kind: task
status: done
---
# Yellow logo on the chest lid

- request: "Il faut le rajouter sur le top, mais en effet prépare une tâche à part" (about the yellow lid logo missing on the flattened chest) (owner, 2026-10-24)
- goal: Add the yellow Kessler-Voss mark on top of the chest lid, as on the validated sheet, and show it in game.
- acceptance: BP_BonusChest shows the yellow mark on the lid top as on vault/assets/chest-turnaround.webp, moving with the lid; lid pivot, chest size and the LockLight and Interior slots unchanged; renders and a -game before | after frame sent to the owner; committed.
- gdd_check: art-prompts-chest, kessler-voss-logo, color-code read (2026-10-24). No conflict: company yellow is allowed, never orange. Open values, defaults pending the owner: Q1 mark = the simplified yellow mark of the sheet (no text), not the full logo artwork (the name is illegible at this size, kessler-voss-logo); Q2 method = the mark cut into the lid top faces as its own material slot Logo with a flat yellow material (sharp at any distance), not painted into the base colour texture; Q3 position and size = as on the sheet, centred on the lid top panel, about 25 x 12 cm, readable from the game camera (lock face at the bottom of the screen); Q4 budget = at most 150 extra tris on the lid; Q5 colour = company yellow of color-code, flat material, no emissive.
- confirmed_by_owner: yes (owner 2026-10-24: "ok : je garde la commande debug, j'accepte les 3864 tris, le pivot et la pose ouverte se règlent dans 309, et ok pour tous les défauts de la tâche 314." Defaults Q1 to Q5 apply; not launched yet)
- milestone: M2
- next_step: none
- started: 2026-11-05T23:11:43Z
- waiting_for: none
- executor_tokens: input 224, cache write 238614, cache read 9506838, output 62650
- executor_usage: runs 2, turns 110, context start 17k peak 133k, resumes 0, 18 min, units 1.56 M
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Done. The 22 x 9.7 cm yellow mark (owner go 2026-11-05) is cut into the lid top as slot Logo (+40 tris, pivot and bounds unchanged, commit faddb143); SM_Chest_Lid reimported with M_ChestLogo (company yellow, roughness 0.45, no emissive) on that slot, Lid transform and the Interior and LockLight slots unchanged (cb5ff37a, bp-bonus-chest updated); in game the mark shows centred on the closed lid at the drop spot from the game camera and closer (before|after sheet Saved/Captures/ChestLogo/before_after_full.png). Not seen on the open lid (it belongs to the lid mesh, so it moves with it). Lid-top shading streaks from the Meshy source remain, out of scope.
- note: task-314-01 runs after task-309-07, which re-cuts the same lid (pivot on the hinge pins); the mark is cut into that lid.

## Subtasks
1. task-314-01-chest-lid-logo (vault/reference/tasks/task-314-chest-lid-logo/task-314-01-chest-lid-logo.md) — 3d: cut the mark into the lid mesh
2. task-314-02-bp-chest-lid-logo (vault/reference/tasks/task-314-chest-lid-logo/task-314-02-bp-chest-lid-logo.md) — bp: reimport the lid and give the Logo slot its yellow material
3. task-314-03-capture-chest-lid-logo (vault/reference/tasks/task-314-chest-lid-logo/task-314-03-capture-chest-lid-logo.md) — cpp: capture the chest in game

## Relations
- [[tasks-done-index]] — parent index.
- [[task-314-01-chest-lid-logo]] — step 01, 3d.
- [[task-314-02-bp-chest-lid-logo]] — step 02, bp.
- [[task-314-03-capture-chest-lid-logo]] — step 03, cpp.
