---
id: task-001-03-arena-grid-cpp
description: "Implement ARRArenaGrid: load a layout JSON by name and build floor and block instances with HISM in OnConstruction."
parent: task-001-arena-m1
kind: task
status: in-progress
---
# C++: ARRArenaGrid

- goal: a C++ actor that shows any valid layout in the editor viewport, in grey box.
- agent: cpp
- depends_on: task-001-01-validator-void
- inputs: [[sys-grid]], [[arena-grid]], [[level-layout-schema]], [[cpp-standard]], [[naming]], [[cpp-bp-split]].
- expected_output: `Source/RogueRigs/Grid/RRArenaGrid.h/.cpp` (`ARRArenaGrid`, Blueprintable). Editable properties: `LayoutName` (file `Content/RogueRigs/Data/Levels/<name>.json`); `CellSize` (default 200 uu); `BlockHeight` (default 100 uu, provisional); `FloorThickness` (default 200 uu, so the asteroid edge reads as a cliff); one mesh and one material per type (floor, rock, ore, heal_ore, special_ore, barrel). Builds in `OnConstruction`: one HISM per type; one floor instance per non-void cell; one block instance per block cell on top of its floor; nothing on void cells; the spawn cell is floor. Collision on floor and blocks. Grid origin at the actor; cell (x, y) center at ((x+0.5)·CellSize, (y+0.5)·CellSize). Public `GetCellCenter(x, y)` and `GetSpawnCell()`. A parse error is logged with the file name; a missing file never crashes. Uses the Json and JsonUtilities modules (Build.cs change allowed for those two only). No destruction yet. [[sys-grid]] updated to match (void, floor, properties).
- done_when: command-line build exits 0 with the editor closed; no new warnings in touched files; [[sys-grid]] matches the code; lint clean.
- editor_state: closed
- token_ceiling: 80000
- milestone: M1
- result: open

## Relations
- [[task-001-arena-m1]] — parent global task.
