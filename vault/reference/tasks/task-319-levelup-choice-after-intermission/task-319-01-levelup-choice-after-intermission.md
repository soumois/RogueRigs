---
id: task-319-01-levelup-choice-after-intermission
description: "Automation test with the real WBP_HUD and HUD swap reproducing level-ups that queue no choice after the intermission; fails on current code, cause confirmed."
parent: task-319-levelup-choice-after-intermission
kind: task
status: in-progress
---
# Reproduction test: level-up choice after the intermission

- goal: rewrite the automation test `RogueRigs.Integration.LevelUpChoiceAfterIntermission` (commit 1d79bd37) so it reproduces the bug WITH the HUD, then confirm the cause. Owner answer 2026-10-26: "Oui, test avec HUD (Recommended)". The test creates the real `WBP_HUD` (class /Game/RogueRigs/UI/WBP_HUD) bound to the miner, as in a game, and goes through `ARRPlayerController::HandleNextLevelReady` (Player/RRPlayerController.cpp ~93-110), which replaces the HUD (or, if that cannot run in the test, destroys the old HUD the same way) before the level-2 level-up; it then expects a queued choice. Cause to confirm (first run, see result): the old `WBP_XPStatus` Destruct calls `UnbindXP(Miner)`, a ClearDelegate that removes ALL `OnLevelUp` listeners, including `URRLevelUpComponent::HandleLevelUp`, bound once in BeginPlay; the miner is kept across levels since task-318. Do not fix gameplay code or Blueprints here. If the HUD test still passes, stop and report.
- agent: cpp
- depends_on: none
- inputs: wbp-xp-status (vault/reference/tech/blueprints/ui/wbp-xp-status.md), wbp-hud (vault/reference/tech/blueprints/ui/wbp-hud.md), sys-hud (vault/reference/tech/systems/ui/sys-hud.md), sys-hud-events (vault/reference/tech/systems/ui/sys-hud-events.md), sys-player (vault/reference/tech/systems/core/sys-player.md), sys-level-up (vault/reference/tech/systems/core/sys-level-up.md), sys-progression (vault/reference/tech/systems/core/sys-progression.md), sys-run (vault/reference/tech/systems/world/sys-run.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-run-transition-fade (vault/reference/tech/systems/world/sys-run/sys-run-transition-fade.md), sys-run-transition-preload (vault/reference/tech/systems/world/sys-run/sys-run-transition-preload.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), sys-tests-progression (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-progression.md), sys-tests-run-integration (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-run-integration.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md), intermission (vault/reference/gdd/run/intermission.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: the test in Source/RogueRigs/Tests/RRIntegrationTests.cpp rewritten with the real WBP_HUD and the HUD swap, on the fade path and the loading path; its test doc node (sys-tests-run-integration) updated; cause confirmed in result
- done_when: build passes; the test with the HUD runs and FAILS on the current code on both paths (no choice queued for the level-2 level-up, and `OnLevelUp` no longer bound to `HandleLevelUp` after the HUD swap); the cause is confirmed in result (asset, function, node); no gameplay code or Blueprint changed; existing tests unchanged in outcome (known pre-existing failures JuiceKillDropsStreamingCubes and LevelUpUpgradeLoop are unrelated, not fixed here); test doc node updated; lint clean.
- editor_state: closed
- token_ceiling: 60000
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- result: open. First run, superseded by the HUD rewrite: (2026-10-26) test `RogueRigs.Integration.LevelUpChoiceAfterIntermission` built and run headless: it PASSES on both paths (level 2 started: blocked 0; level 2 level-up after the delay: blocked 0, pending 1, awaiting 1), so it does not reproduce the bug and the C++ block / `ResumeChoices` path is not the cause. Stopped as the node asks; no gameplay code changed. Best-supported hypothesis (not proven by a test): `ARRPlayerController::HandleNextLevelReady` (Player/RRPlayerController.cpp) removes the old `WBP_HUD` and creates a new one; the old HUD's `WBP_XPStatus` Destruct calls `UnbindXP(Miner)`, which per [[wbp-xp-status]] "unbinds all OnXPChanged and OnLevelUp listeners" of the miner's progression (a `K2Node_ClearDelegate` node in the asset), so `URRLevelUpComponent::HandleLevelUp` loses its `OnLevelUp` binding (bound once in BeginPlay; the miner is kept across levels since task-318). Owner log fits: level 2 had 5 level-ups and no choice, yet the bonus chest choice (`QueueBonusChoice`, not through `OnLevelUp`) was queued and picked (13 cards at level 2 start, 14 in the level 2 snapshot, +2 shop buys = 16). The headless test has no HUD, so it passes. To reproduce: create and bind `WBP_HUD` in the test, then go through `HandleNextLevelReady` (or destroy the old HUD) before the level 2 level-up.

## Relations
- [[task-319-levelup-choice-after-intermission]] — parent global task.
- [[task-319-02-fix-levelup-after-intermission]] — the bp fix that follows, using the cause written in result.
- [[task-319-03-verify-levelup-after-intermission]] — reruns this test after the fix.
