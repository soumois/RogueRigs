---
id: task-319-levelup-choice-after-intermission
description: "Global task: after the first intermission, level-ups no longer offer upgrade cards; reproduce with a test, find the cause, fix."
parent: tasks-done-index
kind: task
status: done
---
# Level-ups give nothing after the intermission

- request: "J'ai eu un bug où aprés le level 15 ( peut être avant ), je ne gagnais plus rien au passage de niveau, est-ce que tu peux regarder à ça ? C'était ma derniére run". To the proposal of writing a test that reproduces it: "lance le test" (owner, 2026-10-26). Q1 "fix right after the reproduction, same task, no capture, no other new test?": "Oui (Recommended)". Q2, after the headless test passed: "Oui, test avec HUD (Recommended)" = rewrite the test with the real WBP_HUD and the HUD swap so it fails, then fix the cause (owner, 2026-10-26).
- evidence (2026-10-26): owner log `Saved/Logs/RogueRigs.log` (PIE 22:37-22:46): levels 13-17 gained in run level 2; 16 cards at the start of level 3 instead of 21, the 5 level-up choices of level 2 missing; the bonus chest choice of level 2 was queued (it uses QueueBonusChoice, not OnLevelUp). No empty card pool. First test (commit 1d79bd37, headless, no HUD) passes: the C++ CancelChoices / ResumeChoices path is not the cause. Suspected cause: `ARRPlayerController::HandleNextLevelReady` (Player/RRPlayerController.cpp ~93-110) destroys the old WBP_HUD; its child WBP_XPStatus Destruct calls `UnbindXP(Miner)`, a ClearDelegate that removes ALL OnLevelUp listeners, including `URRLevelUpComponent::HandleLevelUp` bound once in BeginPlay; the miner is kept across levels since task-318, so nothing rebinds.
- goal: every level-up after an intermission queues an upgrade choice again, as in level 1.
- acceptance: the HUD test fails on the current code and the cause is confirmed; after the WBP_XPStatus fix it passes on both transition paths (fade and loading); other tests keep their outcome (JuiceKillDropsStreamingCubes and LevelUpUpgradeLoop fail before this task, unrelated, not fixed); level-up choices before the intermission unchanged; docs updated; lint clean; committed.
- gdd_check: [[xp-level-up]] read (2026-10-26): "Each level-up offers 3 upgrades", "Level-up keeps across the 4 levels of a run": the bug contradicts the GDD, no conflict with the request. [[intermission]] read: no rule blocks choices after it. No `open` value needed. M2 scope. The test is requested by the owner. No capture: nothing changes on screen. Fix location (Planner, 2026-10-26, not a design choice): in the widget, which clears listeners it does not own; no C++ rebind workaround.
- confirmed_by_owner: yes (2026-10-26): step 1 "lance le test" and Q2 "Oui, test avec HUD"; fix Q1 "Oui (Recommended)".
- milestone: M2
- next_step: none
- result: Done. HUD test passes on fade and loading paths (HandleLevelUp stays bound 1 after HUD swap, level-2 level-up queues a choice). Full suite 273 tests: 271 pass, 2 known unrelated failures (JuiceKillDropsStreamingCubes, LevelUpUpgradeLoop). No pre-fix per-test list, so unchanged outcome rests on only those 2 failing. Commit e5f2b530.

## Subtasks
1. task-319-01-levelup-choice-after-intermission — cpp, editor closed: rewrite the test with WBP_HUD and the HUD swap; it fails; cause confirmed. No gameplay or Blueprint change.
2. task-319-02-fix-levelup-after-intermission — bp, editor open: WBP_XPStatus UnbindXP removes only its own two handlers; doc wbp-xp-status.
3. task-319-03-verify-levelup-after-intermission — cpp, editor closed: rerun the HUD test (passes on both paths) and the suite.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-319-01-levelup-choice-after-intermission]] — step 1, cpp.
- [[task-319-02-fix-levelup-after-intermission]] — step 2, bp.
- [[task-319-03-verify-levelup-after-intermission]] — step 3, cpp.
