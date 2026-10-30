---
id: task-397-shuttle-stays-gone
description: "Global task: After the end-of-level takeoff, the extractor never shows again at its start spot behind the stats, shop or upgrade screens."
parent: tasks-to-do-index
kind: task
status: todo
---
# Extractor stays gone after its takeoff

- request: Owner, 2026-10-30, relayed by the main session (translated): "When the shuttle that leaves at the end of a level takes off, it must not come back to its starting point. Currently I see it teleport back to its position from before it took off while I was on the upgrade screen. (Probably the animation ending.)" Context (main session): BP_ExtractionShuttle_C takes off (spin-up, rise 400 uu in 1.5 s, pitch 20 deg), logs "out of the player view after 3.0 s, removed"; the stats, shop and upgrade screens follow in the same world (task-318 preload), paused. Planner reading: the cause is not known. Candidates to check by a -game run: (a) the takeoff end or the "removed" step resets or only hides the actor and something restores it (paused tick, preload ClearingLevel, transform reset); (b) the next level's arrival extractor (task-371) placed at the new start by the preload during the cards screen and seen behind a non-opaque screen, which the owner would take for the same shuttle; (c) another actor. The fix keeps every extractor out of sight from the takeoff until the next level starts; the arrival extractor still stands next to the miner when the next level starts ([[level-flow]]). One logical point, one global. Not in scope: the backdrop of the upgrade screen (task-398).
- goal: From the end-of-level takeoff until the next level starts, no extractor is visible at its landing spot or at the next start spot behind the stats, shop or upgrade screens; the takeoff look and the next level arrival are unchanged.
- acceptance: cpp: a -game run (rr.Run.EndLevel or real boarding, then rr.Run.Stats.Next, wait on the cards screen, rr.Run.Continue) logs every extractor actor (exit and arrival) with location, hidden state and phase at each step, and the change summary says which one the owner saw and why; fixed so none is visible between the takeoff exit and the next level start (log); arrival extractor present and taking off at the next level start; build passes with the editor closed; docs updated; gdd extraction already holds the rule; lint clean; committed by path; owner checks on screen himself.
- gdd_check: Read 2026-10-30: [[extraction]] (provisional, task 369-377: the extractor takes off behind the end screen until it leaves the camera view): the owner rule "never comes back" written there 2026-10-30, no conflict. [[level-flow]] (arrival: the miner starts next to the extractor, which takes off when the fade in ends): kept. [[intermission]]: stats screen over the paused level, cards screen opaque black (but see task-398 Q12). Tech read: [[sys-objectives-shuttle]] (StartTakeoff, tick when paused, removal once out of view, TakeoffMaxSeconds 12), [[sys-objectives-run-flow]] (PlaceArrivalShuttle, ArrivalShuttle), [[sys-run-transition-preload]] (ClearingLevel destroys the shuttle; PlacingMiner), [[bp-extraction-shuttle]]. Open values: none. Scope M2. Repo state 2026-10-30: task-391-04 (cpp, editor closed) is running; this task runs after it. Config/DefaultGame.ini and Config/DefaultEditor.ini hold uncommitted owner changes: never commit them (commit by path). No packaged build (owner). No capture subtask (owner tests himself).
- confirmed_by_owner: yes (2026-10-30, owner request relayed by the main session; no open value)
- milestone: M2
- next_step: task-397-01-extractor-out-of-sight
- started: open
- waiting_for: none
- result: open

## Subtasks
1. task-397-01-extractor-out-of-sight (vault/reference/tasks/task-397-shuttle-stays-gone/task-397-01-extractor-out-of-sight.md) — cpp: find which extractor shows at its start spot during the intermission and keep it out of sight until the next level
2. task-397-02-doc-extractor-out-of-sight (vault/reference/tasks/task-397-shuttle-stays-gone/task-397-02-doc-extractor-out-of-sight.md) — doc: update the shuttle, run flow and preload docs with the out-of-sight rule

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-397-01-extractor-out-of-sight]] — step 1, cpp.
- [[task-397-02-doc-extractor-out-of-sight]] — step 2, doc.
- [[extraction]] — the takeoff rule this task enforces.
- [[sys-objectives-shuttle]] — the takeoff and removal code.
- [[task-398-upgrade-screen-backdrop]] — the screen behind which the owner saw it.
