---
id: task-390-01-return-to-menu-code
description: "Cpp: BlueprintCallable return to the main menu: run reset then L_MainMenu, menu map setting, debug command"
parent: task-390-run-won-back-to-main-menu
kind: task
status: todo
---
# Run won: back to the main menu: cpp

- goal: Add a BlueprintCallable entry (on URRRunFlowSubsystem next to RestartLevel, or on URRRunTransitionSubsystem; the agent chooses and says why) that calls URRRunSubsystem::ResetRun then opens URRRunSettings::MainMenuMapName (new Config setting, category Run|Menu, default /Game/RogueRigs/Maps/L_MainMenu); the menu controller sets its own UI input at BeginPlay; add a non-shipping debug command (for example rr.Run.MainMenu) calling it. No widget change.
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-objectives-run-flow (vault/reference/tech/systems/world/sys-objectives/sys-objectives-run-flow.md), sys-run (vault/reference/tech/systems/world/sys-run.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-main-menu (vault/reference/tech/systems/ui/sys-main-menu.md), main-menu (vault/reference/gdd/run/main-menu.md), run-structure (vault/reference/gdd/run/run-structure.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), global request and gdd_check: vault/reference/tasks/task-390-run-won-back-to-main-menu.md
- expected_output: C++ entry, setting and debug command, log line `run won: back to the main menu, opening <map>` (or equivalent); commit `task-390-01-return-to-menu-code: ...`; change summary with the function name and signature the bp subtask calls.
- done_when: Build passes with the editor closed; a -game run reaching the level 4 run-won screen (rr.Run.NextLevel x4, rr.Run.Stats.Next) then the debug command logs the run reset (secured ore 0), LoadMap of L_MainMenu and `main menu shown`; rr.Menu.NewRun from there logs level 1/4; uncommitted Config changes untouched; committed by path.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-390-run-won-back-to-main-menu]] — parent global task.
