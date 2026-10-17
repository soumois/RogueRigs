---
id: task-206-02-async-layout
description: "cpp: Asynchronous layout provider."
parent: task-206-level-to-level
kind: task
status: done
---
# async-layout

- goal: Asynchronous layout provider: RequestLayout(level) parses the layout JSON off the game thread and fires OnLayoutReady; ARRArenaGrid builds from a provided layout, falling back to LayoutName when none (direct PIE); the provider interface is the slot the future C++ generator fills.
- agent: cpp
- depends_on: task-206-01-run-state
- inputs: intermission (vault/reference/gdd/run/intermission.md), sys-grid (vault/reference/tech/systems/world/sys-grid.md), sys-grid-api (vault/reference/tech/systems/world/sys-grid/sys-grid-api.md), level-layout-schema (vault/reference/tech/pipelines/level-layout-schema.md), sys-run (vault/reference/tech/systems/world/sys-run.md, written by task-206-01), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: C++ in Source/RogueRigs/ (Run/ and Grid/), sys-run and sys-grid docs updated.
- done_when: build succeeds; existing tests pass; a -game log shows a layout requested and ready before use, and direct PIE on L_ArenaM1 still builds arena_m1_fine; docs updated; committed.
- editor_state: closed
- token_ceiling: 60000
- model: opus
- tokens: input 78, cache write 99219, cache read 2773011, output 2166
- runs: 1
- milestone: M2
- result: IRRLayoutProvider + FRRJsonLayoutProvider on UE::Tasks (2c1e7e94, docs ddda9d67, new sys-run-layout): RequestLayout / IsLayoutReady / OnLayoutReady on URRRunSubsystem, requested at AdvanceToNextLevel; ARRArenaGrid builds from the prepared layout on level 2+ in a run, else its own LayoutName. -game: arena_l2 ready 110 ms after the request off the game thread, level 2 built on arena_l2; direct start builds arena_m1_fine. Tests 260 pass, 2 known pre-existing failures. Unexercised: failed-layout fallback, wait-at-build path.

## Relations
- [[task-206-level-to-level]] — parent global task.
