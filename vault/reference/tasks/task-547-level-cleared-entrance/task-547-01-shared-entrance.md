---
id: task-547-01-shared-entrance
description: "Cpp: Extract the death screen entrance into one shared effect, death screen unchanged"
parent: task-547-level-cleared-entrance
kind: task
status: done
---
# Level cleared screen appears with the death screen entrance effect: cpp

- goal: Extract the death screen entrance into one shared effect, death screen unchanged
- agent: cpp
- depends_on: none
- inputs: death-screen (vault/reference/gdd/run/death-screen.md), intermission (vault/reference/gdd/run/intermission.md)
- expected_output: see done_when
- done_when: A4 and A5 hold; the effect can drive another screen with its title, frame and button parts.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-547
- usage: included in the executor run of task-547
- runs: 1
- milestone: M2
- started: 2026-11-14T03:59:58Z
- waiting_for: none
- result: URRScreenEntrance (UI/RRScreenEntrance.h/.cpp, c7245422): shared entrance of the end screens, parts by name (backdrop, title, title group, frame group, buttons, skip button created on the root canvas when absent), timings in URRScreenEntranceSettings (Config Game, death-screen gdd values), level desaturation moved from URRRunLostWidget, restored at widget destruct; URRRunLostWidget and URRIntermissionWidget drive it from NativeTick; debug rr.Debug.PressKey. Build 0 errors; tests DeathRunLostScreen, Player.GameOver, LevelUpChoiceAfterIntermission, ConsoleExtractionWin pass. Death screen unchanged checked by capture in 547-03 (before frames Saved/Captures/t547_rl_before).

## Relations
- [[task-547-level-cleared-entrance]] — parent global task.
