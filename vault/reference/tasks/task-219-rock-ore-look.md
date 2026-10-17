---
id: task-219-rock-ore-look
description: "Global task: Image proposals for the look of rock blocks in their damage states and of unmined ore blocks, seen from the game camera"
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Rock and ore look: proposals

- request: "Cette tâche sera orienté pour trouver des solutions pour représenter graphiquement la roche ( ses plusieurs états ) et les minerais lorsqu'ils doivent encore être miné" then "Tu peux déjà lancé la tache 218, tant que tu ne touches pas au moteur" (owner, 2026-10-17)
- goal: Image proposals for the look of rock blocks in their damage states and of unmined ore blocks, seen from the game camera
- acceptance: 01 images (2 tracks, rock intact and two damage states, Duskite, heal ore) sent to the owner; 02 style-guide review with every fail shown to the owner and his choice recorded; 03 tech note on rendering the states in the engine, child of sys-grid; no engine, C++, Blueprint or asset change; lint clean; committed.
- gdd_check: [[color-code]] (rock light grey tinted violet with cracks, Duskite same base + glowing violet crystals, heal ore red), [[visual-style]] (low-poly, steep camera, low blocks, M1 grey box), [[ore-blocks]], [[heal-ore]], [[arena-grid]] (240 x 240 cells of 67 uu), [[sys-grid]] (autotiling 6 shapes and top variants approved, not built; per-cell health exists). Owner answers 2026-10-17: states = intact plus damage states by remaining health, cracked = already damaged (DRGS video, [[rock-look-reference]]); defaults accepted: Duskite and heal ore only (special_ore undefined, open), exploration only, block triangle budget open (proposed by 03). Not in this task: weaker rock placed by Perlin noise (owner decision, separate task, its "protection" meaning open). Task number: the owner said 218, taken by task-218-shield-emblem in another session.
- confirmed_by_owner: yes (2026-10-17)
- milestone: M2
- next_step: task-219-03-rock-render-note
- result: open

## Subtasks
1. task-219-01-rock-ore-look — art: image proposals.
2. task-219-02-rock-ore-review — style-guide: review of the images.
3. task-219-04-rock-look-c-fix — art: correction of image C (2 images).
4. task-219-05-rock-look-c-review — style-guide: review of the correction.
5. task-219-03-rock-render-note — cpp: tech note on the dual grid, no code (runs last).

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-219-01-rock-ore-look]] — step 1, art.
- [[task-219-02-rock-ore-review]] — step 2, style-guide.
- [[task-219-04-rock-look-c-fix]] — step 3, art correction.
- [[task-219-05-rock-look-c-review]] — step 4, style-guide.
- [[task-219-03-rock-render-note]] — step 5, cpp.
- [[rock-look-reference]] — the owner DRGS captures behind this task.
