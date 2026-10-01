---
id: task-003-arena-80-paths
description: "Global task: M1 arena grows to 80 x 80 and becomes a network of paths walled by rock; map follow-ups; project renamed Rogue Rigs."
parent: tasks-index
kind: task
status: todo
---
# M1 arena 80 × 80, paths between rocks

- request: "Concernant la répartition : tu vois sur l'image que les rochers sont forts sur les côtés, il faut plutôt des chemins entourés de rochers, tu peux aussi passer à 80x80." Also: rename the project (owner approved).
- goal: `arena_m1` is an 80 × 80 asteroid whose floor is a network of paths walled by rock spread over the whole asteroid (no thick rock rim around an empty center); `L_ArenaM1` shows it with the player start on the new spawn and without the sky light warning; the project is named Rogue Rigs.
- acceptance: (1) the validator passes on `arena_m1` 80 × 80 with its brief; (2) orchestrator measures: every asteroid cell within 5 cells of a floor cell, no solid rock square larger than 6 × 6, longest straight outline edge ≤ 6, one asteroid piece; (3) `L_ArenaM1` HISM counts match the layout, PlayerStart on the spawn cell, no sky light warning in the viewport; (4) PIE spawns `BP_TwinStickCharacter` on the spawn cell; (5) `DefaultGame.ini` `ProjectName=Rogue Rigs`; (6) lint clean, every subtask committed.
- gdd_check: read [[arena-grid]] (M1 arena 60 × 60, approved) → conflict, owner changes it to 80 × 80. [[sys-grid]] repeats the size → replaced by a link to [[arena-grid]]. [[stress-test-m1]] does not depend on arena size. Open values scaled with the area (× 1.78), proposed: asteroid 3600 to 4600 cells, rock coverage kept at 60 % [0.57, 0.63], heal ore 10 to 16. Not in the GDD: the skylight from the template uses real-time capture without an atmosphere, so the viewport shows an error.
- confirmed_by_owner: no
- milestone: M1
- next_step: task-003-01-layout-80-paths
- result: open

## Subtasks
1. task-003-01-layout-80-paths — level-design: `arena_m1` at 80 × 80, path network.
2. task-003-02-map-update — bp: PlayerStart on the new spawn, sky light without real-time capture, counts and PIE check.
3. task-003-03-project-name — cpp: `ProjectName=Rogue Rigs` in `DefaultGame.ini`.

## Relations
- [[tasks-index]] — parent index.
- [[task-003-01-layout-80-paths]] — step 1, level-design, the layout.
- [[task-003-02-map-update]] — step 2, bp, the map.
- [[task-003-03-project-name]] — step 3, cpp, project name.
