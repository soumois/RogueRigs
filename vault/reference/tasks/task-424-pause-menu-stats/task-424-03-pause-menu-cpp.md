---
id: task-424-03-pause-menu-cpp
description: "Cpp: pause input and flow, stats snapshot (global and per weapon), widget base with Resume and Exit, rr.Debug.PauseMenu"
parent: task-424-pause-menu-stats
kind: task
status: done
---
# Pause menu: miner stats screen with Resume and Exit: cpp

- goal: Implement [[pause-menu]] Opening, Content and Buttons as decided (Q1 to Q3, Q6): pause input action in IMC_Miner (Escape, P, gamepad Start), refused while another screen pauses the game or in the main menu; a URRPauseMenuWidget C++ base exposing a stats snapshot (miner attributes and global weapon bonuses, one entry per owned weapon from URRWeaponAttributeSet) and Resume / Exit functions; Exit reuses the run-won Main Menu path; console command rr.Debug.PauseMenu open|resume|exit calling the same functions; widget class property set by the bp subtask. Starts only after the owner validated the mockup.
- agent: cpp
- depends_on: task-424-01-pause-menu-kie-mockup
- inputs: pause-menu (vault/reference/gdd/run/pause-menu.md), player-stats (vault/reference/gdd/player/player-stats.md), main-menu (vault/reference/gdd/run/main-menu.md), sys-main-menu (vault/reference/tech/systems/ui/sys-main-menu.md), sys-gas-core (vault/reference/tech/systems/core/sys-gas-core.md), sys-weapons (vault/reference/tech/systems/combat/sys-weapons.md), sys-hud (vault/reference/tech/systems/ui/sys-hud.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), agent-cpp (vault/reference/agents/agent-cpp.md)
- expected_output: C++ in Source/RogueRigs/ (Menu or UI), build clean, existing tests of the touched families passing, no new automated test; a change summary (classes, properties, input action, command) for the doc subtask; commit by path.
- done_when: Editor and game targets build with no warning; existing automation tests of touched families pass; rr.Debug.PauseMenu open pauses and creates the widget class when set, resume unpauses, exit opens L_MainMenu with the run reset (log lines); committed by path.
- editor_state: Unreal closed during the build
- token_ceiling: 1.2 M units
- model: opus
- tokens: input 78, cache write 144090, cache read 4239637, output 47644
- usage: turns 39, context start 13k peak 150k, resumes 0, 16 min, units 0.84 M
- runs: 1
- milestone: M3
- started: 2026-11-03T06:38:46Z
- waiting_for: none
- result: Commit 31462564: ARRPlayerController pause flow (Escape/P/Start transient IA), URRPauseMenuWidget + FRRPauseMenuStats, rr.Debug.PauseMenu. Both targets build clean, RogueRigs.Player 4/4. Not verified in -game: rr.Debug.PauseMenu open/resume/exit and real key presses, left to task-424-06 capture. Run-flow log line still says "run won" for pause Exit.

## Relations
- [[task-424-pause-menu-stats]] — parent global task.
