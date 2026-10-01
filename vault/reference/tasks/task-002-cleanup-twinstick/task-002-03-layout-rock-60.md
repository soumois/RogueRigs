---
id: task-002-03-layout-rock-60
description: "Rewrite arena_m1 at about 60 percent rock coverage, keeping the irregular asteroid outline, heal ore and reachability rules."
parent: task-002-cleanup-twinstick
kind: task
status: todo
---
# Layout: arena_m1 at 60 % rock

- goal: a denser `arena_m1` that reads as a network of corridors and small pockets inside rock.
- agent: level-design
- depends_on: none
- inputs: [[level-layout-schema]], [[arena-grid]], [[heal-ore]], [[destruction-rules]], [[task-001-02-layout-arena-m1]] (previous version and its constraints).
- expected_output: `Content/RogueRigs/Data/Levels/arena_m1.json` and `arena_m1.brief.json` rewritten. Same constraints as task-001-02 (irregular outline, no straight edge longer than 6 cells, at least 2 void cells to the grid edge, one connected asteroid of about 2000 to 2600 cells, spawn near the center with its 5 × 5 area clear, types `void`, `rock`, `heal_ore`, `spawn` only, heal ore 6 to 10 and none within 6 cells of the spawn, no isolated single rock cell) except: brief `rock_coverage` [0.57, 0.63]. Open floor forms corridors at least 2 cells wide in most places plus a few open pockets; no huge solid mass without any corridor through it (no solid rock square larger than 10 × 10). `reachable_floor_min` 0.85.
- done_when: `node Tools/validate_layout.js Content/RogueRigs/Data/Levels/arena_m1.json --brief Content/RogueRigs/Data/Levels/arena_m1.brief.json` exits 0; orchestrator checks: longest straight edge ≤ 6, one asteroid piece, no solid rock square larger than 10 × 10.
- editor_state: any
- token_ceiling: 30000
- milestone: M1
- result: open

## Relations
- [[task-002-cleanup-twinstick]] — parent global task.
