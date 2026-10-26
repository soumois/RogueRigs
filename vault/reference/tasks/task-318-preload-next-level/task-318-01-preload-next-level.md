---
id: task-318-01-preload-next-level
description: "Prepare the next level in the background from the cards screen, with readiness and 0-1 progress API, no blocking load at Continue."
parent: task-318-preload-next-level
kind: task
status: done
---
# Preload the next level during the cards screen

- goal: The next level loads in the background while the cards screen is shown; Continue fades into it when ready, else shows the loading screen with a progress bar. This subtask covers the background preparation and its readiness and progress API only; the fade and loading paths are task-318-02. No new test (owner rule): only fix existing tests the change breaks.
- agent: cpp
- depends_on: task-317-05-capture-stats-cards
- inputs: intermission (vault/reference/gdd/run/intermission.md), sys-run (vault/reference/tech/systems/world/sys-run.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-run-layout (vault/reference/tech/systems/world/sys-run/sys-run-layout.md), sys-grid (vault/reference/tech/systems/world/sys-grid.md), sys-spawner-pool-prewarm (vault/reference/tech/systems/enemies/sys-spawner/sys-spawner-pool-prewarm.md), sys-objectives-run-flow (vault/reference/tech/systems/world/sys-objectives/sys-objectives-run-flow.md), sys-player (vault/reference/tech/systems/core/sys-player.md), sys-run-log (vault/reference/tech/systems/world/sys-run-log.md), sys-hud-bindings (vault/reference/tech/systems/ui/sys-hud/sys-hud-bindings.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: C++ in Source/RogueRigs/ (Run and the systems it resets): next level prepared in the background from the moment the cards screen opens, readiness and 0-1 progress API; updated docs sys-run-transition, sys-run-layout and every system doc touched
- done_when: build passes; opening the cards screen starts the full preparation of the next level (layout, terrain/grid, actors and objectives reset, pool prewarm, miner snapshot ready) without a blocking map load at Continue, by rebuilding in the same world or by any approach that reaches the same result (choice and reason written in sys-run-transition); IsNextLevelReady() and GetNextLevelProgress() 0-1 (BlueprintPure) plus an OnNextLevelReady event; restart and death still reset the run to level 1; carried state (cards, weapons, level, XP, health, ore) identical to the map-travel path; a -game log shows the preparation start, its duration and readiness before Continue; existing tests broken by the change updated and passing; lint clean.
- editor_state: closed
- token_ceiling: 100000
- model: opus
- tokens: input 178, cache write 213924, cache read 11862235, output 68096
- runs: 2
- milestone: M2
- result: Next level prepared in the same world from the cards screen (layout, grid, actors and objectives reset, prewarm, miner snapshot), no map travel; IsNextLevelReady, GetNextLevelProgress, OnNextLevelReady; approach in sys-run-transition-preload; build passes, 270 tests pass (2 known unrelated failures); -game log shows level 2 ready before Continue. Terrain rebuild ~0.5 s in one frame freezes the cards screen once (owner to decide on time-slicing). Commits bd0bbf66, a2d45398.

## Relations
- [[task-318-preload-next-level]] — parent global task.
