---
id: sys-subtitles
description: "Subtitle subsystem as built: one voice line at a time with speaker prefix and C++ timer, company alert hooks, delegates the HUD subtitle widget binds."
parent: systems-ui-index
kind: doc
---
# Subtitles

- responsibility: show voice lines as one-line subtitles (task-099-01): one line at a time, a new line replaces the current one (queue of one), "<Speaker>: <line>", timed in C++ so the widget never polls. Serves the five company alert lines; barks (Rook, Vesna) can use `Show` later, nothing shows them yet.
- main_classes (`Source/RogueRigs/Text/`): `URRSubtitleSubsystem` (`UTickableWorldSubsystem`, Game and PIE worlds, `Config = Game`), `FRRSubtitleSpeaker` {`Id`, `DisplayName`}.
- entry_points: `Show(SpeakerId, DisplayName, Text, DurationOverride = -1)` (empty name = table name, no prefix for an unknown speaker; duration <= 0 = formula; empty text refused); `ShowAlertLine(EventId)` (picks with `URRTextContentSubsystem::GetAlertLine`, speaker `kessler_voss`); `Clear()`; pure getters `HasSubtitle`, `GetCurrentSpeakerId`, `GetCurrentSpeaker`, `GetCurrentText`, `GetCurrentSubtitle`, `GetCurrentDuration`, `GetRemainingSeconds`, `GetSpeakerDisplayName`, `GetDurationForText`; static `Get(WorldContext)`, `ComposeSubtitle`.
- events (BlueprintAssignable): `OnSubtitleShown(SpeakerId, Speaker, Text, Subtitle, Duration)` on every show, replace included; `OnSubtitleCleared()` when the time runs out or on `Clear`, never between a replace.
- run transition (task-207-01): `Initialize` binds `URRRunTransitionSubsystem` ([[sys-run-transition]]); `OnIntermissionStarted` / `OnLoadingStarted` clear the current line (`OnSubtitleCleared`) and silence; `Show` then returns false and drops the line (no queue, nothing pops up later) until `OnLevelReady`; a world created while the phase is not None starts silenced. Pure `IsSilenced()`. Bare test worlds without a game instance never silence.
- run end (task-211-01): `OnWorldBeginPlay` binds `URRRunFlowSubsystem::OnRunEnded` (end screen: run won on the last level, shuttle missed; a level 1-3 extraction does not broadcast it and is silenced by the intermission) and the auth `ARRGameMode::OnGameOver` (miner death), and logs `world begin play, subtitles allowed|silenced (run end bound|unbound, game over bound|unbound)`. Either clears the current line and silences for the rest of the world (`bRunEndSilenced`: `OnLevelReady` does not lift it); log `subtitles silenced (run ended, run won | run ended, shuttle missed | game over, miner dead)`. The restart (`RestartLevel`, game mode restart timer) opens a new world: a new subsystem, allowed. `Deinitialize` unbinds.
- hooks: `URRRunFlowSubsystem::PickAlertLine` (console_dropped, bonus_chest_dropped, extraction_ready, shuttle_countdown once per `LandShuttle`) and `URRHUDWidget::HandleHordeIncoming` (horde_incoming) call `ShowAlertLine` ([[sys-text-content]], [[sys-hud-pressure]]). The display widget is a WBP_HUD sub-widget (task-099-02, [[wbp-hud]]).
- data: `Speakers` (kessler_voss "Kessler-Voss", rook "Rook", vesna "Vesna"), `BaseSeconds` 4, `SecondsPerCharacter` 0.06 (characters of the line, prefix excluded); config properties, owner decision 2026-10-10.
- depends_on: [[sys-text-content]].
- implements: owner decisions of task-099 (company voice lines as subtitles).
- gotchas: the timer uses world time and freezes while the game is paused. extraction_ready and shuttle_countdown fire in the same frame (`CheckExtraction` then `LandShuttle`): the countdown line replaces the extraction line at once. Tests: `RogueRigs.Subtitles.*`, `RogueRigs.Text.AlertEventsWired` ([[sys-automation-tests]]).

## Relations
- [[systems-index]] — parent index.
- [[sys-text-content]] — picks the lines it shows.
- [[sys-hud-pressure]] — horde banner hook.
- [[wbp-hud]] — hosts the subtitle widget.
- [[sys-automation-tests]] — its tests.
- [[sys-run-transition]] — silences it during the intermission and the loading.
- [[sys-objectives-run-flow]] — OnRunEnded silences it.
- [[sys-player]] — the game over (miner death) silences it.
