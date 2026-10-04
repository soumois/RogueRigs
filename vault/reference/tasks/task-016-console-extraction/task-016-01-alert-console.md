---
id: task-016-01-alert-console
description: "Run flow: alert bar from time and kills, paused while the console is pending; console at 40 %, marked rocks, drone hack, drain; tests."
parent: task-016-console-extraction
kind: task
status: done
---
# Alert bar and console

- goal: the alert bar and the console objective work in C++.
- agent: cpp
- depends_on: task-015-02-bp-shotgun
- inputs: [[level-flow]], [[difficulty]], [[objective-console]], [[drone]], [[destruction-rules]], [[sys-difficulty]], [[sys-drone]], [[sys-grid]], [[sys-flow-field]], [[sys-hud]], [[sys-automation-tests]], [[cpp-standard]].
- expected_output: run flow owner (e.g. `URRRunFlowSubsystem` or on the game mode): alert 0→1 in 240 s, +0.5 s per kill, paused while a mandatory objective is pending; at 0.4 a console actor drops on a reachable floor cell 12 to 20 cells from the miner; rock blocks within 2 cells are flagged marked (event for the outline); hacking possible only when no marked rock remains; the drone switches to hack when the miner is within 3 cells (one target at a time, no dig meanwhile), 20 s to complete, drains 5 %/s while the miner is out; completion unpauses the alert; delegates for the HUD (alert value, console progress, objective state); a cheat to speed up the alert. Values editable or data. Docs: [[sys-difficulty]] (counter owner), new `sys-objectives` system doc, [[sys-drone]], [[sys-hud]]. Tests `RogueRigs.RunFlow.*` and `RogueRigs.Console.*`.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 130000
- model: opus
- tokens: input 144, cache write 250509, cache read 11239785, output 115999
- runs: 2
- milestone: M1
- result: URRRunFlowSubsystem (alert 240 s + 0.5 s per kill, kamikaze self-destruct not a kill, stops at 0.4 and drops the console, paused while pending, OnExtractionReady once when full with no objective); ARRConsole (12–20 cells straight line, reachable, prefers cells with rocks to mark (bPreferCellsWithMarkedRocks, owner to review), rock-type blocks within 2 cells marked via instance custom data slot 0 and OnCellMarkedChanged, hack 20 s in a 3-cell zone, drain 5 %/s, drone hack state refuses digs); values on BP_RRGameMode RunFlowConfig and the console class. BP events: console OnMarkedRocksChanged, OnHackProgress, OnHackStateChanged; HUD OnAlertChanged, OnObjectiveChanged, OnObjectiveProgressChanged, OnExtractionReady. URRCheatSettings CDO property route polled each frame (GrantXP, bInvulnerable, ChooseUpgrade, bReroll, AlertSpeedMultiplier, bSkipToConsole, bCompleteConsoleHack, bSkipToExtraction) + matching CVars. 6 new tests; build 0, 88/88. Docs sys-objectives (new), sys-difficulty, sys-drone, sys-hud, sys-cheats, sys-grid, sys-automation-tests, unreal-mcp. Commits 0ddd8c6, e1c5e04; about 480k tokens over two runs (ceiling 130k).

## Relations
- [[task-016-console-extraction]] — parent global task.
