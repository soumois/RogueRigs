---
id: task-230-08-rock-damage-data
description: "Damage display data: per-quadrant custom data, shake in the material, plate overlay and ore crystal HISMs."
parent: task-230-rock-slab-integration
kind: task
status: done
---
# Damage display data: per-quadrant custom data, shake in the material, plate overlay and ore crystal HISMs

- goal: Damage display data: per-quadrant custom data, shake in the material, plate overlay and ore crystal HISMs.
- agent: cpp
- depends_on: task-230-07-dual-grid-placement
- inputs: vault/reference/tech/systems/world/sys-grid/rock-state-render-options/rock-render-damage.md, vault/reference/tech/systems/world/sys-grid/rock-state-render-options/rock-render-rebuild.md, vault/reference/tech/systems/world/sys-grid/rock-state-render-options/rock-render-top-texture.md, vault/reference/tech/systems/world/sys-grid.md, vault/reference/tech/systems/tools/sys-automation-tests.md; code Source/RogueRigs/Grid/
- expected_output: 4 custom data floats per piece (damage d per quadrant, d > 0 as soon as a cell is below 100 %), shake written as custom data instead of moving instances, broken piece variant swapped in for a piece as soon as one of its 4 corner cells has d > 0 (owner 2026-10-18, rock-render-damage), back never (damage only rises), one crystal cluster instance per ore and heal cell in their own chunk HISMs (Duskite: SM_OreChunk_01; heal: same mesh, heal material property), weak rock not included; tests updated.
- done_when: build exits 0; all RogueRigs.* tests pass headless; docs updated; committed.
- editor_state: closed
- token_ceiling: 100000
- model: opus
- tokens: input 138, cache write 232682, cache read 10961709, output 31765
- runs: 2
- milestone: M2
- result: Commits 4294ed2f (code, tests), 1f79d576 (docs: sys-grid-rock-damage, sys-grid-rock-piece-data), 977b899d. 13 custom data floats per piece (d, shake, marked per quadrant, solid mask); broken variant swap at first damage, never back; shake as data; outline restored; ore and heal crystal cluster per cell (OreCrystalMesh SM_OreChunk_01, scale 1.5 provisional). Empty quadrant d = max of solid 4-neighbours (provisional). Build 0; tests 262/264, the 2 known pre-existing failures. Open: sag inputs beyond the 4 quadrants approximated in 09; crystals do not follow the sag.

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
