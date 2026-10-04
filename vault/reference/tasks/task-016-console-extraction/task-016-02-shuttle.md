---
id: task-016-02-shuttle
description: "Full alert with no pending objective lands the shuttle 25+ cells away with a 45 s countdown; reaching it wins, missing it loses; restart; tests."
parent: task-016-console-extraction
kind: task
status: done
---
# Shuttle extraction

- goal: extraction ends the level with a win or a loss.
- agent: cpp
- depends_on: task-016-01-alert-console
- inputs: [[extraction]], [[level-flow]], [[run-structure]], [[sys-difficulty]], [[sys-hud]], [[sys-automation-tests]], [[cpp-standard]].
- expected_output: shuttle actor landing on a reachable floor cell at least 25 cells from the miner when the alert is full and no objective is pending; 45 s countdown; miner overlap → success; countdown end → run lost (robots keep coming meanwhile); both pause gameplay and raise an end event (win or loss) for the HUD; `RestartLevel()` reloads the map. Docs: `sys-objectives`, [[sys-hud]]. Tests `RogueRigs.Extraction.*`: landing distance, countdown loss, overlap win, no landing while the console is pending.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 90000
- model: opus
- tokens: input 66, cache write 119422, cache read 2609795, output 48468
- runs: 1
- milestone: M1
- result: ARRShuttle landed on OnExtractionReady at a reachable floor cell ≥ 25 cells straight line (fallback farthest reachable), 45 s countdown, win within 1 cell (ShuttleBoardRadiusCells, provisional), loss at 0, robots keep spawning; miner death freezes it. EndRun cancels and blocks level-up offers, pauses via player 0, UI input, OnRunEnded(bSuccess); RestartLevel reopens the map. BP events: shuttle OnLanded, OnCountdown, OnExtracted, OnMissed; HUD OnShuttleCountdown(Remaining, ShuttleLocation), OnRunEnded(bSuccess), callable RestartLevel. RunFlowConfig ShuttleClass to set by BP. Cheat ShuttleCountdownOverride. 5 new Extraction tests; build 0, 93/93. Real pause to check in PIE. Ore not secured (ore economy not built). Docs sys-objectives, sys-hud, sys-cheats, sys-level-up, sys-automation-tests. Commits faebb7c, 0300351; about 125k tokens.

## Relations
- [[task-016-console-extraction]] — parent global task.
