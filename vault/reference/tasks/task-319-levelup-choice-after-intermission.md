---
id: task-319-levelup-choice-after-intermission
description: "Global task: after the first intermission, level-ups no longer offer upgrade cards; reproduce with a test, find the cause, fix."
parent: tasks-to-do-index
kind: task
status: todo
---
# Level-ups give nothing after the intermission

- request: "J'ai eu un bug où aprés le level 15 ( peut être avant ), je ne gagnais plus rien au passage de niveau, est-ce que tu peux regarder à ça ? C'était ma derniére run". To the proposal of writing a test that reproduces it: "lance le test" (owner, 2026-10-26). Fix after the reproduction: Q1 pending, default yes (see Subtasks).
- evidence (read-only investigation, 2026-10-26, nothing modified): last run log `Saved/Logs/RogueRigs.log` (PIE 22:37-22:46). Miner levels 13-17 were gained in run level 2, 18-19 in run level 3. No "no upgrade card could be offered" error, so the card pool did not empty. "snapshot kept in place" lines: 13 cards at the start of level 2 (11 level-ups + 1 bonus chest + 1 shop buy, consistent); 16 at the start of level 3 instead of 21 (13 + 5 level-ups + 1 chest + 2 shop buys): the 5 level-up choices of level 2 are missing. Flow: URRProgressionComponent::AddXP -> OnLevelUp -> URRLevelUpComponent::HandleLevelUp (heal, then QueueChoice unless bChoicesBlocked). URRRunFlowSubsystem::EndRun (RRRunFlowSubsystem.cpp ~611) calls LevelUp->CancelChoices(), which sets bChoicesBlocked = true; the only unblock is URRLevelUpComponent::ResumeChoices(), called from URRRunTransitionSubsystem::StartPreparedLevel (RRRunTransitionSubsystem.cpp ~872). That path looks wired, yet the block seems to stay on; the run postdates task-318 (preload, fade or loading path). Cause not confirmed.
- goal: every level-up after an intermission queues an upgrade choice again, as in level 1.
- acceptance: a test reproduces the bug on the current code (fails), the cause is written; after the fix the test passes on both transition paths (fade and loading); existing tests pass; level-up choices before the intermission unchanged; docs updated; lint clean; committed.
- gdd_check: [[xp-level-up]] read (2026-10-26): "Each level-up offers 3 upgrades", "Level-up keeps across the 4 levels of a run": the bug contradicts the GDD, no conflict with the request. [[intermission]] read: no rule blocks choices after it. No `open` value needed. M2 scope, a regression of task-318 if confirmed. The test is requested by the owner ("lance le test"), not unrequested work. No capture: nothing changes on screen.
- confirmed_by_owner: yes (2026-10-26, "lance le test") for step 1; step 2 runs on Q1 (default yes).
- milestone: M2
- next_step: task-319-01-levelup-choice-after-intermission
- result: open

## Subtasks
1. task-319-01-levelup-choice-after-intermission — cpp: reproduction test, failing, and the cause. Changes no gameplay code.
2. task-319-02-fix-levelup-after-intermission — cpp: the fix; the test passes. Runs only if step 1 reproduced the bug and Q1 is yes.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-319-01-levelup-choice-after-intermission]] — step 1, cpp.
- [[task-319-02-fix-levelup-after-intermission]] — step 2, cpp.
