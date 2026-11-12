---
id: task-509-01-merge-old-options
description: "Executor: bring the old-workflow options screen onto master, remove the new-workflow one, renumber the old task to 510"
parent: task-509-options-old-workflow-on-master
kind: task
status: done
---
# Écran Options : version de l'ancien workflow sur master, puis push: executor

- goal: Code, Blueprints, assets, images and task nodes of the old version on master in place of the new version, nothing else of master lost.
- agent: executor
- depends_on: none
- inputs: task-504-options-screen-look (vault/reference/tasks/task-504-options-screen-look.md), task-505-options-screen-sound-graphics (vault/reference/tasks/task-505-options-screen-sound-graphics.md), task-506-options-gameplay-and-pause (vault/reference/tasks/task-506-options-gameplay-and-pause.md), options-menu (vault/reference/gdd/run/options-menu.md), C:/Workspace/RogueRigs-old (branch exp/old-workflow-options, e0968216, base c77dc729)
- expected_output: see done_when
- done_when: A1, A2 (code and assets), A3 and A4 hold; editor build succeeds, automation tests pass, the game starts without the audio crash (A5); committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-509
- usage: included in the executor run of task-509
- runs: 1
- milestone: M2
- started: 2026-11-12T12:27:40Z
- waiting_for: none
- result: 1abc6245. Source, Content, Art and Tools set to the exp/old-workflow-options tree at e0968216 (git restore; every master change under them since c77dc729 came from tasks 504-506): old URRGameUserSettings, URROptionsSettings, URROptionsSubsystem, URROptionsWidget, Settings/RROptionsDebugCommands (rr.Options.*), WBP_Options, WBP_MainMenu, WBP_PauseMenu, SC_Master/Music/Effects, nine T_Options_* sprites and their Art/UI PNGs, Art/Source/options-screen mockups and prompts 01-06; removed the 504-506 files (RRSettingsDebugCommands, UI/RROptionsDebugCommands, Mix_Volumes, T_Options_ArrowButton/TabPlate/ToggleThumb/ToggleTrack, create_volume_sound_classes.py, their Art/Source images and prompt 02-elements-sheet); RRPlayerController.h and RRPauseMenuDebugCommands.cpp back to the base. Maps and Config identical to master (git diff empty). Old task imported as task-510-options-screen (11 subtasks, ids and links renumbered, original results kept with a note that commits and Saved/Captures paths are on the branch and worktree), listed in tasks-done-index, its 11 cost rows added. The old vault docs were brought in this subtask too, to keep lint clean (options-screen, sys-options, wbp-options and the menu, hover, damage-numbers and art-prompt nodes; options-menu, sys-game-settings, sys-options-screen removed). Build: RogueRigsEditor Development exit 0 (only the pre-existing C4996 Cast warning of RRGroundTelegraphTests.cpp). Tests: RogueRigs.DamageNumbers (10) and RogueRigs.Pickups.OreNumber, 11/11 Success, exit 0. Game launch on L_MainMenu (Saved/Captures/t509_launch): no crash, no fatal line, exit 0.

## Relations
- [[task-509-options-old-workflow-on-master]] — parent global task.
