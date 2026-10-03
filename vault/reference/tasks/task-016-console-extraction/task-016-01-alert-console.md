---
id: task-016-01-alert-console
description: "Run flow: alert bar from time and kills, paused while the console is pending; console at 40 %, marked rocks, drone hack, drain; tests."
parent: task-016-console-extraction
kind: task
status: todo
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
- milestone: M1
- result: open

## Relations
- [[task-016-console-extraction]] — parent global task.
