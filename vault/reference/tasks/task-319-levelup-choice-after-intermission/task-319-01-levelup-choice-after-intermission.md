---
id: task-319-01-levelup-choice-after-intermission
description: "Automation test with the real WBP_HUD and HUD swap reproducing level-ups that queue no choice after the intermission; fails on current code, cause confirmed."
parent: task-319-levelup-choice-after-intermission
kind: task
status: done
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
- model: opus
- tokens: input 194, cache write 298773, cache read 5166904, output 56444
- usage: turns 97, context start 11k peak 108k, resumes 0, 21 min, units 1.17 M
- runs: 1
- milestone: M2
- result: HUD test fails on both paths as expected (OnLevelUp->HandleLevelUp bound 1, then 0 after old HUD Destruct; level-2 level-up queues no choice). Cause confirmed: WBP_XPStatus Destruct -> UnbindXP -> K2Node_ClearDelegate on OnLevelUp removes all listeners incl. URRLevelUpComponent::HandleLevelUp. Test swaps HUD itself (no local player headless). Commits af5c0ed9, 98561ec5.

## Relations
- [[task-319-levelup-choice-after-intermission]] — parent global task.
- [[task-319-02-fix-levelup-after-intermission]] — the bp fix that follows, using the cause written in result.
- [[task-319-03-verify-levelup-after-intermission]] — reruns this test after the fix.
