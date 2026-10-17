---
id: task-206-03-transition-flow
description: "cpp: Level transition."
parent: task-206-level-to-level
kind: task
status: in-progress
---
# transition-flow

- goal: Level transition: success on levels 1 to 3 enters an intermission phase (game paused, next layout requested at once, OnIntermissionStarted with recap data: level index, ore secured this level, run total); Continue opens the arena map for the next level; a loading phase lasts until the layout is ready, the grid built and the pool prewarm done (OnLoadingStarted, OnLevelReady), and the run flow starts only then; level 4 success shows the run-won end screen; a loss shows run-lost and restart starts a new run at level 1. Blueprint events and C++ entry points for the two widgets.
- agent: cpp
- depends_on: task-206-02-async-layout
- inputs: intermission (vault/reference/gdd/run/intermission.md), run-structure (vault/reference/gdd/run/run-structure.md), extraction (vault/reference/gdd/run/extraction.md), sys-run (vault/reference/tech/systems/world/sys-run.md, written by task-206-01), sys-objectives-run-flow (vault/reference/tech/systems/world/sys-objectives/sys-objectives-run-flow.md), sys-objectives-events (vault/reference/tech/systems/world/sys-objectives/sys-objectives-events.md), sys-spawner-pool-prewarm (vault/reference/tech/systems/enemies/sys-spawner/sys-spawner-pool-prewarm.md), sys-actor-pool (vault/reference/tech/systems/world/sys-actor-pool.md), sys-hud (vault/reference/tech/systems/ui/sys-hud.md), sys-hud-bindings (vault/reference/tech/systems/ui/sys-hud/sys-hud-bindings.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: C++ in Source/RogueRigs/, sys-run, sys-objectives-run-flow and sys-hud docs updated; the events and functions the bp widgets bind to, listed in sys-run.
- done_when: build succeeds; existing tests pass; a -game log (with cheat SkipToExtraction) shows level 1 success, intermission, Continue, loading, level 2 on arena_l2 with the carried state, and level 4 success ending the run; docs updated; committed.
- editor_state: closed
- token_ceiling: 80000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-206-level-to-level]] — parent global task.
