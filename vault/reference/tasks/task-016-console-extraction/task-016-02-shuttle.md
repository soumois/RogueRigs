---
id: task-016-02-shuttle
description: "Full alert with no pending objective lands the shuttle 25+ cells away with a 45 s countdown; reaching it wins, missing it loses; restart; tests."
parent: task-016-console-extraction
kind: task
status: todo
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
- milestone: M1
- result: open

## Relations
- [[task-016-console-extraction]] — parent global task.
