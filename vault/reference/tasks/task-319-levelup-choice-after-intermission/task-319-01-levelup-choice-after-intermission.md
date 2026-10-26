---
id: task-319-01-levelup-choice-after-intermission
description: "Automation test reproducing level-ups that queue no upgrade choice after the intermission, run failing on current code, cause identified; no gameplay fix."
parent: task-319-levelup-choice-after-intermission
kind: task
status: todo
---
# Reproduction test: level-up choice after the intermission

- goal: write and run an automation test that reproduces the bug (level-ups after the intermission queue no choice), then find the cause in code. Evidence and suspected flow (EndRun -> CancelChoices sets bChoicesBlocked; ResumeChoices only in StartPreparedLevel; task-318 fade and loading paths): `evidence` in the parent node. Do not fix gameplay code here. If the test passes on the current code, stop and report: the reproduction is wrong, ask before anything else.
- agent: cpp
- depends_on: none
- inputs: sys-level-up (vault/reference/tech/systems/core/sys-level-up.md), sys-progression (vault/reference/tech/systems/core/sys-progression.md), sys-run (vault/reference/tech/systems/world/sys-run.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-run-transition-fade (vault/reference/tech/systems/world/sys-run/sys-run-transition-fade.md), sys-run-transition-preload (vault/reference/tech/systems/world/sys-run/sys-run-transition-preload.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), sys-tests-progression (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-progression.md), sys-tests-run-integration (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-run-integration.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md), intermission (vault/reference/gdd/run/intermission.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: one automation test in Source/RogueRigs/Tests/ (RRLevelUpTests.cpp or RRIntegrationTests.cpp) that ends a level by extraction, goes through the intermission into the next level (fade path and loading path), gains a level and expects a queued upgrade choice; its test doc node updated; cause written in result
- done_when: build passes; the new test runs and FAILS on the current code, reproducing the bug (no choice queued after the intermission); the cause is identified in code (file, function, why bChoicesBlocked stays true or the choice is lost) and written in result; no gameplay code changed; existing tests still pass; test doc node updated; lint clean.
- editor_state: closed
- token_ceiling: 60000
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- result: (2026-10-26) test `RogueRigs.Integration.LevelUpChoiceAfterIntermission` built and run headless: it PASSES on both paths (level 2 started: blocked 0; level 2 level-up after the delay: blocked 0, pending 1, awaiting 1), so it does not reproduce the bug and the C++ block / `ResumeChoices` path is not the cause. Stopped as the node asks; no gameplay code changed. Best-supported hypothesis (not proven by a test): `ARRPlayerController::HandleNextLevelReady` (Player/RRPlayerController.cpp) removes the old `WBP_HUD` and creates a new one; the old HUD's `WBP_XPStatus` Destruct calls `UnbindXP(Miner)`, which per [[wbp-xp-status]] "unbinds all OnXPChanged and OnLevelUp listeners" of the miner's progression (a `K2Node_ClearDelegate` node in the asset), so `URRLevelUpComponent::HandleLevelUp` loses its `OnLevelUp` binding (bound once in BeginPlay; the miner is kept across levels since task-318). Owner log fits: level 2 had 5 level-ups and no choice, yet the bonus chest choice (`QueueBonusChoice`, not through `OnLevelUp`) was queued and picked (13 cards at level 2 start, 14 in the level 2 snapshot, +2 shop buys = 16). The headless test has no HUD, so it passes. To reproduce: create and bind `WBP_HUD` in the test, then go through `HandleNextLevelReady` (or destroy the old HUD) before the level 2 level-up.

## Relations
- [[task-319-levelup-choice-after-intermission]] — parent global task.
- [[task-319-02-fix-levelup-after-intermission]] — the fix that follows, using the cause written in result.
