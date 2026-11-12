---
id: sys-game-settings
description: "Player settings C++ as built: URRGameUserSettings Sound and Graphics options, volume groups, display keep-or-revert, URROptionsSettings data, rr.Settings."
parent: systems-ui-index
kind: doc
---
# Game settings (Sound and Graphics)

- responsibility: holds the player options of [[options-menu]], applies each change at once, saves it per Windows user and applies it again at launch. A window mode or resolution change waits for Keep and reverts by itself. No widget of its own: [[sys-options-screen]] draws it.
- main_classes (`Source/RogueRigs/Settings/`, task-505-01, 661107d9):
  - `URRGameUserSettings` (`UGameUserSettings`, `Config = GameUserSettings`): set as the engine class by `Config/DefaultEngine.ini` `GameUserSettingsClassName=/Script/RogueRigs.RRGameUserSettings`. Static `GetRRGameUserSettings()`. Also holds `bShowDamageNumbers` ([[sys-damage-numbers]]).
    - Sound: `GetVolume` / `SetVolume(ERRVolumeGroup Master|Music|Effects, 0..1)` (snapped to `VolumeStep`), `ResetSoundToDefaults`, `ApplyVolumes` (pushes `Mix_Volumes` with the three class overrides on the main audio device; again after each map load through `PostLoadMapWithWorld`).
    - Display (confirmed): `SyncDisplayFromWindow` (reads the real window, a `-windowed` launch differs from the file), `RequestWindowMode`, `StepWindowMode`, `GetResolutionOptions` (monitor modes, or windowed sizes), `IsResolutionSelectable` (false in Windowed fullscreen), `RequestResolution`, `StepResolution`. Each starts the confirmation: `IsDisplayConfirmPending`, `GetDisplayConfirmSecondsLeft`, `ConfirmDisplayChange` (Keep), `RevertDisplayChange` (Revert, Escape, end of countdown, ticker), event `OnDisplayConfirmChanged(bool)`. While pending, `SaveSettings` writes the previous mode and resolution.
    - Applied at once: `SetVSyncOn`, `SetFrameRateLimitValue` / `StepFrameRateLimit` (0 = Unlimited), `GetQualityPreset` (-1 = Custom when the five details differ), `SetQualityPreset` / `StepQualityPreset`, `Get/SetGraphicsDetail(ERRGraphicsDetail Shadows|Effects|AntiAliasing|PostProcessing|Textures)`, `Get/SetResolutionScalePercent`, `ResetGraphicsToDefaults` (first-launch values; display part through the confirmation).
    - Text helpers for the widget: `GetWindowModeText`, `GetResolutionText`, `GetFrameRateLimitText`, `GetQualityLevelText`, `GetOnOffText`, `GetPercentText`, `GetShownResolution`; `DescribeSettings` (dump line).
  - `URROptionsSettings` (`UObject`, `Config = Game`, section `[/Script/RogueRigs.RROptionsSettings]` of `DefaultGame.ini`): sound class and mix paths, `VolumeStep` 0.05, `FrameRateLimitOptions` 30, 60, 120, 144, 0, resolution scale 50..100 step 5, `DisplayConfirmSeconds` 15 (provisional).
  - `RRSettingsDebugCommands.cpp`: `rr.Settings dump | set <master|music|effects|mode|res|vsync|fps|preset|shadows|fx|aa|pp|textures|scale> <value> | keep | revert | reset <sound|graphics>`.
- entry_points: the setters above (called by [[sys-options-screen]]); `rr.Settings`.
- data: assets `/Game/RogueRigs/Audio/SC_Master` (parent of `SC_Music` and `SC_Effects`), `Mix_Volumes`, made by `Tools/unreal/create_volume_sound_classes.py`; saved values in the user's `GameUserSettings.ini` under `Saved/Config` (section `[/Script/RogueRigs.RRGameUserSettings]`). First-launch values in [[options-menu]].
- depends_on: engine scalability groups (`sg.*`), `r.VSync`, `t.MaxFPS`, `r.ScreenPercentage`.
- implements: [[options-menu]] Sound, Graphics, Persistence.
- gotchas:
  - No sound exists yet ([[audio-direction]]): volumes are checked by read-back of the effective class volumes only.
  - Verified (task-505-01, -game L_MainMenu, `Saved/Captures/task505_settings_run1`, `run2`): engine read-back matches every change, values reloaded after restart, countdown ran out and reverted, Keep and Revert work.

## Relations
- [[systems-ui-index]] — parent index.
- [[options-menu]] — the design this implements.
- [[sys-options-screen]] — the screen that edits these settings.
- [[sys-damage-numbers]] — the other setting held by this class.
- [[audio-direction]] — why the volume groups have no sound yet.
- [[task-505-options-screen-sound-graphics]] — the task that built it.
