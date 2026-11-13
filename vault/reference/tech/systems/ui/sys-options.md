---
id: sys-options
description: "Options C++ as built: settings class, options subsystem (sound mix, keep-or-revert, focus loss), options widget base, menu wiring, rr.Options commands; open for any setting."
parent: systems-ui-index
kind: doc
---
# Options

- responsibility: holds, applies and saves the player settings of [[options-screen]] (sound, display, quality, four gameplay toggles) and runs the options screen opened from the main and pause menus. Task-500 (03 75dfba40, 04 2075f53b, fixes 3c85e594).
- main_classes:
  - `URRGameUserSettings` (`Settings/RRGameUserSettings.h/.cpp`, the engine `GameUserSettingsClassName`): enums `ERRQualityPreset` (Low..Epic, Custom), `ERRQualitySetting` (Shadows, Effects, AntiAliasing, PostProcessing, Textures), `ERRVolumeGroup`, `ERRGameplayToggle`, `ERROptionsTab`.
  - `URROptionsSettings` (`Settings/RROptionsSettings.h/.cpp`, Config=Game, `[/Script/RogueRigs.RROptionsSettings]`): tunables below.
  - `URROptionsSubsystem` (`Settings/RROptionsSubsystem.h/.cpp`, game instance subsystem).
  - `URROptionsWidget` (Abstract, `UI/RROptionsWidget.h/.cpp`) and `URROptionsControlProxy` (carries the key of each control). Blueprint child: [[wbp-options]].
  - `Settings/RROptionsDebugCommands.cpp` (not shipping).
- entry_points:
  - Settings: `SetVolume`, `SetGameplayToggle`, `SetVSyncAndApply`, `SetFrameRateLimitAndApply`, `SetResolutionScalePercent`, `SetQualityPreset`, `SetQualitySetting`, `ResetTabToDefaults(Tab)`, getters, static `IsGameplayToggleOn`. Each setter applies and calls `CommitChange` (SaveSettings + `OnSettingsChanged`).
  - Subsystem: `SetWindowMode`, `SetResolution`, `ResetGraphicsToDefaults`, `KeepDisplay`, `RevertDisplay`, `IsDisplayConfirmPending`, `GetDisplayConfirmSecondsLeft`, `GetResolutionOptions`, `OnDisplayConfirmChanged(bPending)`, `StartDisplayConfirmOnCurrent` (capture).
  - Widget: static `OpenOptions(FromMenu, Tab)`, `FindOpen(World)`, `IsOptionsAvailable`; `ShowTab`, `Back`, `HandleEscape`, `ResetCurrentTab`, `RefreshFromSettings`.
  - Menus: `URRMainMenuWidget::OpenOptions` and `URRPauseMenuWidget::OpenOptions`, both binding `OptionsButton` (BindWidgetOptional, enabled when the class loads, plate hover). `ARRPlayerController::TogglePauseMenu` sends the pause key to the open screen's `HandleEscape`.
  - Console: `rr.Options.Set <name> <value>` (master, music, effects 0-100, windowmode 0-2, resolution WxH, vsync, framelimit, scale, preset 0-3, shadows, effectsquality, antialiasing, postprocessing, textures 0-3, damagenumbers, aimindicator, screenshake, pauseonfocusloss 0/1, reset sound|graphics|gameplay), `rr.Options.Open [sound|graphics|gameplay] [dialog|close]` (opens the pause menu first during a run), `rr.Options.Dump`, `rr.Options.Keep`, `rr.Options.Revert`.
- data:
  - Saved in `GameUserSettings.ini` of the user's Saved/Config (`[/Script/RogueRigs.RRGameUserSettings]`: `bShowDamageNumbers`, `bShowAimIndicator`, `bScreenShake`, `bPauseOnFocusLoss`, `MasterVolume`, `MusicVolume`, `EffectsVolume`; engine display fields; `[ScalabilityGroups]`). In the editor `-game` it is `Saved/Config/WindowsEditor/`; in a packaged game the user's local app data.
  - `URROptionsSettings` defaults: sound classes `/Game/RogueRigs/Audio/SC_Master`, `SC_Music`, `SC_Effects`; `VolumeStep` 0.05; `FrameRateLimits` 30, 60, 120, 144, 0 (Unlimited); resolution scale 50 to 100 step 5; `DisplayConfirmSeconds` 15; `OptionsWidgetClass` `/Game/RogueRigs/UI/WBP_Options`; `OptionsZOrderAbove` 5 (screen at Z 45); tab plates `ActiveTabTexture` T_Shop_ButtonContinue (margin 0.155), `InactiveTabTexture` T_Options_Tab (margin 0.10), tab label colours.
- flow:
  - Sound: a transient sound mix holds class overrides Master = M, Music = M x Mu, Effects = M x E (children not touched), pushed at start, after each map load and each change; the classes are held by the subsystem (the audio device keeps raw pointers).
  - Display: a window mode or resolution change confirms the state on screen first, applies at once, marks the change pending (SaveSettings writes the confirmed values meanwhile) and starts a real-time 15 s ticker (runs while paused). Keep confirms and saves; Revert, Escape, Back or the end of the countdown restores and saves.
  - Quality: a preset sets every scalability group (resolution scale kept); the preset reads Custom when the five sub-settings differ. Resolution scale 0 in the file (engine default) reads 100 %.
  - Gameplay: damage, heal and ore numbers read `bShowDamageNumbers` ([[sys-damage-numbers]]); the aim laser hides when off ([[sys-aim-indicator]]); every camera shake is skipped in `URRCameraShakeLibrary::StartShake`; on application deactivation the first local `ARRPlayerController` opens its pause menu when allowed.
  - Screen: `OpenOptions` collapses the menu, adds the screen at Z 45 with keyboard focus; `Back` reverts a pending display change, removes the screen, restores the menu and focuses its `OptionsButton`.
- depends_on: [[sys-main-menu]], [[sys-pause-menu]], [[sys-button-hover]], [[sys-damage-numbers]], [[sys-aim-indicator]].
- implements: [[options-screen]].
- gotchas:
  - A first launch has no file and keeps the engine values; nothing is benchmarked.
  - The tab buttons do not take the plate hover (their brush is swapped per tab); they brighten by tint.
  - No sound asset exists yet: the volumes are applied to the classes only (task-510-05).
  - Verified: see [[wbp-options]].

## Relations
- [[systems-ui-index]] — parent index.
- [[options-screen]] — the design this implements.
- [[wbp-options]] — the widget Blueprint on `URROptionsWidget`.
- [[sys-main-menu]] — opens the screen from the main menu.
- [[sys-pause-menu]] — opens the screen from the pause menu.
- [[sys-damage-numbers]] — reads the Damage numbers toggle.
- [[sys-aim-indicator]] — reads the Aim indicator toggle.
- [[sys-button-hover]] — hover of the screen buttons.
- [[sys-audio]] — sound events played through the Effects class these sliders set.
