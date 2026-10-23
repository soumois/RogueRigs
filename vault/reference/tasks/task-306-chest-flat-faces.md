---
id: task-306-chest-flat-faces
description: "Global task: Flatten the puffed Meshy chest faces after the sheet, cut it into body, lid and latch, and show it in BP_BonusChest."
parent: tasks-to-do-index
kind: task
status: todo
---
# Bonus chest mesh with flat faces

- request: "j'ai un problème avec le coffre optimisé, il est devenu boursouflé. à l'aide de l'image, raplatit les faces." (owner, 2026-10-23)
- goal: Flatten the puffed Meshy chest faces after the sheet, cut it into body, lid and latch, and show it in BP_BonusChest.
- acceptance: BP_BonusChest shows the owner chest with flat faces matching the sheet (body, lid and latch as separate parts with their pivots, lock light in its own slot), footprint 2 x 2 cells kept; renders and -game frames before and after sent to the owner; committed.
- gdd_check: objective-bonus-chest, art-prompts-chest, color-code read (2026-10-23). No conflict (corporate locker, colours of the sheet). Pending owner answers, defaults: Q1 Blender cleanup of the Meshy mesh (not a rebuild); Q2 at most 3000 tris; Q3 about 130 x 80 x 70 uu, bottom-centre origin, lock face toward the camera; Q4 the cut into body, lid (pivot on the hinge axis) and latch (pivot on its top edge, lock light slot LockLight) is done here once, for task-308 and task-309; Q5 the gold whole-chest glow (GlowAmount) is dropped; Q15 logo plate kept as in the mesh.
- confirmed_by_owner: no (planned 2026-10-23, "ne lance rien pour l'instant"; waiting for the owner's answers, the defaults below apply if he answers "ok")
- milestone: M2
- next_step: task-306-01-chest-flatten
- result: open

## Subtasks
1. task-306-01-chest-flatten — 3d: Flatten the chest faces after the sheet
2. task-306-02-chest-cut — 3d: Cut the chest into body, lid and latch
3. task-306-03-bp-chest-import — bp: Import the chest parts into BP_BonusChest
4. task-306-04-capture-chest-mesh — cpp: Capture the new chest in game

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-306-01-chest-flatten]] — step 01, 3d.
- [[task-306-02-chest-cut]] — step 02, 3d.
- [[task-306-03-bp-chest-import]] — step 03, bp.
- [[task-306-04-capture-chest-mesh]] — step 04, cpp.
