---
id: task-505-01-settings-sound-graphics
description: "Cpp: Sound and Graphics settings of options-menu stored, applied at once and at launch, exposed to the widget."
parent: task-505-options-screen-sound-graphics
kind: task
status: done
---
# Options screen with Sound and Graphics, opened from the main menu: cpp

- goal: Sound and Graphics settings of options-menu stored, applied at once and at launch, exposed to the widget.
- agent: cpp
- depends_on: none
- inputs: options-menu (vault/reference/gdd/run/options-menu.md), audio-direction (vault/reference/gdd/art-audio/audio-direction.md), Source/RogueRigs/Settings/RRGameUserSettings.h
- expected_output: see done_when
- done_when: A3 and A4 of the global hold in a -game run: each graphics change is read back from the engine, volume values reach their groups, values reloaded after a restart; build green, existing tests pass.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-505
- usage: included in the executor run of task-505
- runs: 1
- milestone: M2
- started: 2026-11-11T21:59:16Z
- waiting_for: none
- result: Done (661107d9). URRGameUserSettings: volumes Master/Music/Effects (0..1, step 0.05) pushed through Mix_Volumes on SC_Master (parent) / SC_Music / SC_Effects (new, /Game/RogueRigs/Audio, Tools/unreal/create_volume_sound_classes.py); window mode, resolution with 15 s keep-or-revert (URROptionsSettings), VSync, frame limit, preset + 5 details (Custom), resolution scale; each setter applies and saves; widget helpers (Step*, texts). -game L_MainMenu (Saved/Captures/task505_settings_run1, run2, rr.Settings): engine read-back matches every change (sg.*, r.VSync, t.MaxFPS, r.ScreenPercentage, window), effective volumes master 0.50 music 0.20 effects 0.40, all values reloaded and applied after restart, countdown ran out and reverted Windowed fullscreen to Windowed 1280x720, Revert and Keep work. Build green; verify scope build only (settings, no gameplay family). First-launch: Windowed fullscreen at desktop size, VSync off, Unlimited, Epic, sg.ResolutionQuality 0 (engine default, 100 %).

## Relations
- [[task-505-options-screen-sound-graphics]] — parent global task.
