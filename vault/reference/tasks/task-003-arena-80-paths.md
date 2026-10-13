---
id: task-003-arena-80-paths
description: "Global task: M1 arena grows to 80 x 80 and becomes a network of paths walled by rock; map follow-ups; project renamed Rogue Rigs."
parent: tasks-done-index
kind: task
status: done
---
# M1 arena 80 × 80, paths between rocks

- request: "Concernant la répartition : tu vois sur l'image que les rochers sont forts sur les côtés, il faut plutôt des chemins entourés de rochers, tu peux aussi passer à 80x80." Also: rename the project (owner approved).
- goal: `arena_m1` is an 80 × 80 asteroid whose floor is a network of organic paths walled by rock, with hidden pockets to dig into, spread over the whole asteroid (no thick rock rim around an empty center); `L_ArenaM1` shows it with the player start on the new spawn and without the sky light warning; the project is named Rogue Rigs.
- acceptance: (1) the validator passes on `arena_m1` 80 × 80 with its brief; (2) orchestrator measures: every asteroid cell within 5 cells of a floor cell, longest straight outline edge ≤ 6, one asteroid piece, hidden pockets behind thin rock, reachable floor 60 to 90 %; (3) `L_ArenaM1` HISM counts match the layout, PlayerStart on the spawn cell, no sky light warning in the viewport; (4) PIE spawns `BP_TwinStickCharacter` on the spawn cell; (5) `DefaultGame.ini` `ProjectName=Rogue Rigs`; (6) lint clean, every subtask committed.
- gdd_check: read [[arena-grid]] (M1 arena 60 × 60, approved) → conflict, owner changes it to 80 × 80. [[sys-grid]] repeats the size → replaced by a link to [[arena-grid]]. [[perf-stress-test-m1]] does not depend on arena size. Open values scaled with the area (× 1.78), proposed: asteroid 3600 to 4600 cells, rock coverage kept at 60 % [0.57, 0.63], heal ore 10 to 16. Owner: paths organic with dead ends, hidden pockets reached by digging (reachable floor need not be near 100 %), long rock walls allowed. Not in the GDD: the skylight from the template uses real-time capture without an atmosphere, so the viewport shows an error.
- confirmed_by_owner: yes
- milestone: M1
- next_step: none
- result: all acceptance checks passed. arena_m1 80 × 80 with organic paths, 7 hidden pockets, 59.1 % rock, 12 heal ore; L_ArenaM1 rebuilt, PlayerStart (8100, 8100, 120), sky light warning gone; PIE spawn on the spawn cell; project named Rogue Rigs. Subtask 02 needed a second attempt (external actor files not saved). About 140k tokens.

## Subtasks
1. task-003-01-layout-80-paths — level-design: `arena_m1` at 80 × 80, path network.
2. task-003-02-map-update — bp: PlayerStart on the new spawn, sky light without real-time capture, counts and PIE check.
3. task-003-03-project-name — cpp: `ProjectName=Rogue Rigs` in `DefaultGame.ini`.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-003-01-layout-80-paths]] — step 1, level-design, the layout.
- [[task-003-02-map-update]] — step 2, bp, the map.
- [[task-003-03-project-name]] — step 3, cpp, project name.
