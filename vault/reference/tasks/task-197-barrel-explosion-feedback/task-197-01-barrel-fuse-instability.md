---
id: task-197-01-barrel-fuse-instability
description: "C++: during the barrel fuse the cell shakes more and more (render only) and its fuse progress feeds a per-instance custom data float; cheat rr.Cheat.FuseBarrel."
parent: task-197-barrel-explosion-feedback
kind: task
status: todo
---
# Barrel fuse instability

- goal: during the 1.5 s barrel fuse, the barrel cell shakes in render only with an amplitude that grows with fuse progress (reuse the dig shake path of the grid; start and end amplitudes as provisional `EditAnywhere` values; it replaces the current "shake stopped" at fuse start); the grid writes the fuse progress (0..1, 0 when not fusing) each grid tick into one per-instance custom data float of the barrel HISM instance (index chosen and exposed by a getter, like the marked and ore colour floats; reset on destroy and rebuild) so the barrel body material can blink; one-click test: cheat `rr.Cheat.FuseBarrel 1` and its property route on `URRCheatSettings` (one-shot, MCP cannot set CVars) lights the fuse of the barrel nearest to the player miner and logs the cell and the distance; docs `sys-grid-barrels`, `sys-grid-api`, `sys-cheats` updated.
- agent: cpp
- depends_on: none
- inputs: barrel (vault/reference/gdd/terrain/barrel.md), ground-telegraph (vault/reference/gdd/art-audio/ground-telegraph.md), sys-grid (vault/reference/tech/systems/world/sys-grid.md), sys-grid-barrels (vault/reference/tech/systems/world/sys-grid/sys-grid-barrels.md), sys-grid-api (vault/reference/tech/systems/world/sys-grid/sys-grid-api.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), barrel-asset (vault/reference/tech/pipelines/barrel-asset.md), bp-arena-grid (vault/reference/tech/blueprints/world/bp-arena-grid.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: C++ in `Source/RogueRigs/Grid/` and `Source/RogueRigs/Debug/`, docs; one commit.
- done_when: build exit 0; existing tests pass (only expectations broken by this change updated, no new test); a run log line shows `rr.Cheat.FuseBarrel 1` lighting a barrel fuse and the barrel exploding 1.5 s later; the custom data index and the shake values are named in the docs; committed by path.
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-197-barrel-explosion-feedback]] — parent global task.
