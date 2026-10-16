---
id: task-202-01-crater-first-frame-black
description: "No black crater on the frame an explosion opens it: newly exposed floor renders correctly from its first frame, verified in -game frames."
parent: task-202-crater-first-frame-black
kind: task
status: in-progress
---
# Crater first frame black

- goal: No black crater on the frame an explosion opens it: newly exposed floor renders correctly from its first frame, verified in -game frames.
- agent: cpp
- depends_on: task-201-01-decal-first-frame-green (one build at a time)
- inputs: sys-grid (vault/reference/tech/systems/world/sys-grid.md), sys-grid-barrels (vault/reference/tech/systems/world/sys-grid/sys-grid-barrels.md), sys-grid-api (vault/reference/tech/systems/world/sys-grid/sys-grid-api.md), sys-explosions (vault/reference/tech/systems/combat/sys-explosions.md), bp-arena-grid (vault/reference/tech/blueprints/world/bp-arena-grid.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: C++ fix, docs; frames in `Saved/Screenshots/` (not committed); one or two commits.
- done_when: build exit 0; grid, explosion and barrel tests pass (only broken expectations updated, no new test); the task-200 `-game` command (new prefix) shows no black crater on the explosion frame; the cause found is written in the docs; game rules and the 2 ms per-frame destruction budget unchanged; committed by path.
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-202-crater-first-frame-black]] — parent global task.
