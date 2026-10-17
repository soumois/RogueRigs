---
id: task-230-07-dual-grid-placement
description: "ARRArenaGrid draws rock with dual-grid pieces at cell corners and rebuilds them on destruction."
parent: task-230-rock-slab-integration
kind: task
status: todo
---
# ARRArenaGrid draws rock with dual-grid pieces at cell corners and rebuilds them on destruction

- goal: ARRArenaGrid draws rock with dual-grid pieces at cell corners and rebuilds them on destruction.
- agent: cpp
- depends_on: task-230-01-rock-baseline-capture
- inputs: vault/reference/tech/systems/world/sys-grid/rock-state-render-options.md, vault/reference/tech/systems/world/sys-grid/rock-state-render-options/rock-render-piece-set.md, vault/reference/tech/systems/world/sys-grid/rock-state-render-options/rock-render-rebuild.md, vault/reference/tech/systems/world/sys-grid/rock-state-render-options/rock-render-counts.md, vault/reference/tech/systems/world/sys-grid.md, vault/reference/tech/systems/world/sys-grid/sys-grid-api.md, vault/reference/tech/systems/world/sys-grid/sys-grid-deposits.md, vault/reference/tech/systems/tools/sys-automation-tests.md; code Source/RogueRigs/Grid/
- expected_output: piece HISM per shape and variant per chunk (mesh properties per shape, one variant now, variant index chosen by cell hash, several supported), floor slab under every non-void cell and bBlocksFillFloorSlab false, collision ISM per cell unchanged, index maps rewritten for corner pieces, a broken cell updates its 4 corner pieces, Nanite enabled on piece HISMs, ore and heal cells render as rock pieces; automation tests updated and passing; sys-grid docs updated.
- done_when: build exits 0 editor closed; all RogueRigs.* tests pass headless; docs updated; committed.
- editor_state: closed
- token_ceiling: 120000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
