---
id: task-496-01-generator-provider
description: "Cpp: A run layout provider that generates each level with the C++ generator from a per-run seed, per-level params and brief from packaged content, hand JSON fallback."
parent: task-496-generator-in-runs
kind: task
status: done
---
# Layout generator wired into real runs: cpp

- goal: A run layout provider that generates each level with the C++ generator from a per-run seed, per-level params and brief from packaged content, hand JSON fallback.
- agent: cpp
- depends_on: none
- inputs: sys-layout-generator (vault/reference/tech/systems/world/sys-grid/sys-layout-generator.md), sys-run-layout (vault/reference/tech/systems/world/sys-run/sys-run-layout.md), layout-generator-recipe (vault/reference/tech/pipelines/layout-generator-recipe.md), level-layout-schema (vault/reference/tech/pipelines/level-layout-schema.md), task-492-cpp-layout-generator (vault/reference/tasks/task-492-cpp-layout-generator.md), task-493-async-layout-loader (vault/reference/tasks/task-493-async-layout-loader.md), task-494-generator-test-map (vault/reference/tasks/task-494-generator-test-map.md), Content/RogueRigs/Data/Levels/, Tools/layout_gen/params/fine_default.json
- expected_output: see done_when
- done_when: Acceptance A1, A4, A5 and the seed part of A6 hold in a -game -nullrhi log run through 4 levels (rr.Run.NextLevel or the existing run flow); determinism checked by two generations of one seed; test map and commandlet still work; build clean, existing tests that the change breaks updated.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-496
- usage: included in the executor run of task-496
- runs: 1
- milestone: M2
- started: 2026-11-10T19:50:16Z
- waiting_for: none
- result: Done (commit 4d85a6f3). FRRGeneratorLayoutProvider (Run/RRLayoutProvider) installed by URRRunSubsystem when URRRunSettings::bGenerateRunLayouts (default true): FRRLayoutGenerator::Generate from the level seed (DeriveLevelSeed(run seed, level)), params and brief from URRRunSettings::LevelGeneratorParams/LevelGeneratorBriefs in Content/RogueRigs/Data/Levels; in-memory layout, name kept = hand layout name; hand JSON fallback with an Error line. Run seed random (FGuid) drawn by PrepareNewRunLayout, called by ResetRun (New Run, Retry, Main Menu) unless a prepared run waits; BeginRun keeps it; level 1 grid builds from the prepared layout. Debug cvar rr.Layout.ForceGenFail. Data: arena_l2..l4.params.json got the review section of fine_default (the C++ loader requires it; task-495 values); level 1 = arena_m1_fine.params.json (copy of fine_default) + arena_m1_fine.gen.brief.json = arena_m1_fine brief with heal_ore [56,56], barrel [17,22] (deviation: the literal arena_m1_fine brief [48,48]/[14,18] contradicts the default params of task-491, no seed could pass). Checks (Saved/Logs/task496/): run1.log -game -nullrhi New Run + 3 NextLevel: levels 1-4 generated (run seed 53210213; level seeds 914799536, 533741479, 422704489, 436380369; 0.08/0.08/5.69/0.49 s), Restart drew run seed 764433333 with another level 1 (hash 03ecb6ea vs 8284cdee); no Tools/ path in the log. Determinism: commandlet twice on seed 422704489 with l3 params/brief: accepted 422704499, cells hash 86c36f6d = run value, JSON identical except name. fallback.log: ForceGenFail 2 -> Error line, arena_l2.json used, level 2 started. gentest.log: rr.Level.New 3 ok; commandlet ok. Build clean (one pre-existing Cast C4996 in unity Module.RogueRigs.5). Tests LayoutLoader + Integration LevelUpChoiceAfterIntermission, DeathRunLostScreen, ShuttleTimeoutLoss, ConsoleExtractionWin: 7/7 pass. Cost: record-cost.js found no usage (agent-usage.js reads ~/.claude/projects/C--Workspace-RogueRigs, absent: the session transcripts are under another project folder), closed with task-status.js.

## Relations
- [[task-496-generator-in-runs]] — parent global task.
