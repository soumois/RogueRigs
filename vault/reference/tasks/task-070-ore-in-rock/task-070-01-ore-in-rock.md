---
id: task-070-01-ore-in-rock
description: "Ore scattered through the rock: nuggets inside rock blocks so ore drips during the whole dig, as in DRGS."
parent: task-070-ore-in-rock
kind: task
status: done
---
# Ore scattered in the rock

- goal: Ore scattered through the rock: nuggets inside rock blocks so ore drips during the whole dig, as in DRGS.
- agent: cpp
- depends_on: none
- inputs: ore-blocks (vault/reference/gdd/terrain/ore-blocks.md), drgs-video-2026-10-06 (vault/reference/production/drgs-video-2026-10-06.md), sys-grid (vault/reference/tech/systems/sys-grid.md), sys-drone (vault/reference/tech/systems/sys-drone.md), sys-pickups (vault/reference/tech/systems/sys-pickups.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: design first (GDD check at launch): nugget density per rock block, ore per nugget, layout impact on the 3 × 3 ore veins, drip timing (DRGS: about 3 chunks per second while digging); then the grid data, drone dig and pickup changes; likely cpp then bp subtasks for the look.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 90000
- model: opus
- tokens: input 32, cache write 42858, cache read 452866, output 12865
- runs: 1
- milestone: M2
- result: Design phase only: proposal notes ore-in-rock-proposal and ore-in-rock-plan (commit 67f7bd7); owner reviews the values before any code.

## Relations
- [[task-070-ore-in-rock]] — parent global task.
