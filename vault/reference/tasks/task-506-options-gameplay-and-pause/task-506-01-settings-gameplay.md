---
id: task-506-01-settings-gameplay
description: "Cpp: Gameplay settings of options-menu stored and obeyed by the game systems."
parent: task-506-options-gameplay-and-pause
kind: task
status: done
---
# Options screen: Gameplay tab and access from the pause menu: cpp

- goal: Gameplay settings of options-menu stored and obeyed by the game systems.
- agent: cpp
- depends_on: none
- inputs: options-menu (vault/reference/gdd/run/options-menu.md), damage-numbers (vault/reference/gdd/weapons/damage-numbers.md), aim-indicator (vault/reference/gdd/art-audio/aim-indicator.md)
- expected_output: see done_when
- done_when: A3 and A4 of the global hold in a -game run; build green, existing tests pass.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-506
- usage: included in the executor run of task-506
- runs: 1
- milestone: M2
- started: 2026-11-12T04:46:27Z
- waiting_for: none
- result: Done c72ae69e. URRGameUserSettings ERRGameplayOption (DamageNumbers, AimIndicator, ScreenShake, PauseOnFocusLoss; default On; Get/SetGameplayOption saves; ResetGameplayToDefaults). Aim laser hidden when Off (RefreshIndicator); URRCameraShakeLibrary::StartShake plays nothing when Off (every camera shake goes through it, so Screen shake is kept); ARRPlayerController opens the pause menu on Slate application deactivation when On. Build green; verify_scope: no test family covers the touched code (camera shake library, aim indicator, controller), so build only. -game L_ArenaM1 Saved/Captures/task506_01_run1..4: all four Off stored and read back, no laser in off_00.png, focus loss with Off logs no pause; after restart the dump still reads 0 0 0 0 (A4), reset gameplay restores 1 1 1 1; real window deactivation opened the pause menu (run4) and rr.Debug.PauseMenu focuslost too (run3). Damage numbers and shake Off not seen on a frame here (no hit in the window); their gates are code-read, left to the capture subtask.

## Relations
- [[task-506-options-gameplay-and-pause]] — parent global task.
