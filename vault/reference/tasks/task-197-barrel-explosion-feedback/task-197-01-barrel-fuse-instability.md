---
id: task-197-01-barrel-fuse-instability
description: "C++: during the barrel fuse the cell shakes more and more (render only) and its fuse progress feeds a per-instance custom data float."
parent: task-197-barrel-explosion-feedback
kind: task
status: in-progress
---
# Barrel fuse instability

- goal: during the 1.5 s barrel fuse, the barrel cell shakes in render only with an amplitude that grows with fuse progress (reuse the dig shake path of the grid; start and end amplitudes as provisional `EditAnywhere` values; it replaces the current "shake stopped" at fuse start); the grid writes the fuse progress (0..1, 0 when not fusing) each grid tick into one per-instance custom data float of the barrel HISM instance (index chosen and exposed by a getter, like the marked and ore colour floats; reset on destroy and rebuild) so the barrel body material can blink; no cheat or debug command (owner 2026-10-16); docs `sys-grid-barrels` and `sys-grid-api` updated.
- agent: cpp
- depends_on: none
- inputs: barrel (vault/reference/gdd/terrain/barrel.md), ground-telegraph (vault/reference/gdd/art-audio/ground-telegraph.md), sys-grid (vault/reference/tech/systems/world/sys-grid.md), sys-grid-barrels (vault/reference/tech/systems/world/sys-grid/sys-grid-barrels.md), sys-grid-api (vault/reference/tech/systems/world/sys-grid/sys-grid-api.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), barrel-asset (vault/reference/tech/pipelines/barrel-asset.md), bp-arena-grid (vault/reference/tech/blueprints/world/bp-arena-grid.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: C++ in `Source/RogueRigs/Grid/`, docs; one commit.
- done_when: build exit 0; existing tests pass (only expectations broken by this change updated, no new test); the custom data index and the shake values are named in the docs; committed by path.
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-197-barrel-explosion-feedback]] — parent global task.
