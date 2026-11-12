---
id: task-510-04-options-widget-cpp
description: "Cpp: options widget base: tabs, controls bound to settings, Reset, Back and Escape, display keep-or-revert dialog, opened from both menus."
parent: task-510-options-screen
kind: task
status: done
---
# Options screen: Sound, Graphics and Gameplay settings, look from a Kie.ai mockup: cpp

- goal: options widget base: Sound, Graphics, Gameplay tabs, controls bound to the settings, Reset to defaults, Back and Escape, keep-or-revert display dialog, opening from the main and pause menus with focus
- agent: cpp
- depends_on: task-510-03-options-settings-cpp
- inputs: options-screen (vault/reference/gdd/run/options-screen.md), sys-main-menu (vault/reference/tech/systems/ui/sys-main-menu.md), sys-pause-menu (vault/reference/tech/systems/ui/sys-pause-menu.md), sys-button-hover (vault/reference/tech/systems/ui/sys-button-hover.md)
- expected_output: see done_when
- done_when: Build clean; no Apply button; the main menu and pause menu C++ bases open the options widget and get focus back on Back or Escape; the pause stays on; the dialog "Keep these display settings?" with Keep, Revert and a 15 s countdown shows after a display change, Escape reverts; a debug console command opens the screen on a given tab, and the dialog, for the capture.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-510
- usage: included in the executor run of task-510
- runs: 1
- milestone: M3
- started: 2026-11-12T09:17:43Z
- waiting_for: none
- result: 2075f53b. URROptionsWidget (UI/RROptionsWidget): controls found by name in WBP_Options (tabs SoundTabButton/GraphicsTabButton/GameplayTabButton + TabSwitcher; <Key>Slider/Fill/Value for Master, Music, Effects, ResolutionScale; <Key>Prev/Next/Value selectors for WindowMode, Resolution, FrameLimit, Preset, Shadows, EffectsQuality, AntiAliasing, PostProcessing, Textures; <Key>Toggle/Knob/State for VSync and the four gameplay toggles; ResetButton, BackButton; ConfirmDialog, ConfirmCountdownText, KeepButton, RevertButton), every change applied at once (no Apply). OpenOptions(FromMenu, Tab): class URROptionsSettings::OptionsWidgetClass (/Game/RogueRigs/UI/WBP_Options), Z 45, menu collapsed then restored with focus on its OptionsButton; Escape (or gamepad back) reverts while the dialog shows, else Back; the pause key with options open acts as Escape, game stays paused. Main menu and pause menu bases bind OptionsButton (enabled when the class loads, hover attached). Tab plates: active T_Shop_ButtonContinue, inactive T_Options_Tab (settings). Debug rr.Options.Open [tab] [dialog|close]. Build clean (build only: UI routing).

## Relations
- [[task-510-options-screen]] — parent global task.
