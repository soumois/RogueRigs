---
id: sys-run-transition-fade
description: "Continue fade path (fade out, switch, fade in) and loading path (loading screen with progress, then fade in): events, settings, overlay, log lines."
parent: sys-run-transition
kind: doc
---
# Run transition — fade and loading paths (task-318-02)

- responsibility: what the player sees between `Continue` and the next level when it is prepared in this world ([[sys-run-transition-preload]], [[intermission]]): a fade when the level is ready, the loading screen with progress when it is not.
- main_classes: `URRRunTransitionSubsystem` (`Run/`): `ERRTransitionFade` (None, FadingOut, FadingIn), `StartScreenFade`, `FinishScreenFade`; `URRScreenFadeWidget` (`UI/RRScreenFadeWidget.h`, native `UUserWidget`, no Blueprint: one black `UImage` built in `NativeOnInitialized`); `ARRPlayerController` adds it at `ScreenFadeZOrder` (60, above the transition widgets at 50/51) and pushes `GetScreenFadeAlpha` every frame on the core ticker (runs while paused); `URRLoadingScreenWidget` progress.
- entry_points:
  - Fade path (ready at Continue): shop closed, phase Loading, no `OnLoadingStarted` and no loading screen; `OnScreenFadeStarted(FadingOut, 0.3)`; the cards screen stays under the black; after the fade `OnScreenFadeFinished(FadingOut)`, then `StartPreparedLevel` (switch: unpause, `OnLevelReady`, controller removes the cards screen, HUD and subtitles restored), then `OnScreenFadeStarted(FadingIn, 0.5)`; `OnScreenFadeFinished(FadingIn)` removes the overlay.
  - Loading path (still preparing at Continue): `OnLoadingStarted` (loading screen, HUD and subtitles kept hidden), `OnNextLevelProgressChanged(Progress)` at each preparation step; when ready `StartPreparedLevel`, `OnLevelReady` removes the loading screen, fade in 0.5 s from black.
  - Map-travel fallback ([[sys-run-transition]]): unchanged, plus the same fade in after `OnLevelReady`.
  - Gameplay stays held (paused world, phase Loading, input ignored) until the switch; the fade in plays over the started level.
  - Blueprint API for WBP_Loading: pure `URRLoadingScreenWidget::GetLoadingProgress()` (0-1) and event `OnLoadingProgressChanged(Progress)` (also once at `ShowLoading`); subsystem pure `GetLoadingProgress()`, `GetScreenFade()`, `GetScreenFadeAlpha()`, events `OnScreenFadeStarted(Fade, Duration)`, `OnScreenFadeFinished(Fade)`, `OnNextLevelProgressChanged(Progress)`.
  - Arrival takeoff (task-371-01): `OnScreenFadeFinished(FadingIn)` starts the arrival extractor takeoff when the run flow placed it under the black ([[sys-objectives-run-flow]]); the map-travel fallback path is not verified.
- data: `URRRunSettings::TransitionFadeOutSeconds` (0.3), `TransitionFadeInSeconds` (0.5), provisional (owner 2026-10-26), real time, 0 = cut; controller `ScreenFadeZOrder` (60).
- depends_on: [[sys-run-transition-preload]], [[sys-hud-bindings]].
- implements: [[intermission]].
- gotchas: `GetLoadingProgress` is 0 on the map-travel fallback (no measured progress). Each preparation step is one frame and the terrain step takes about 0.5 s in a single frame, so a progress bar jumps (0.25, 0.5, 0.75, 1) and freezes during the terrain rebuild. Verified by a -game -nullrhi log (Saved/Logs/task318_02_game.log), no visual check: level 2 fade path: Continue frame 43711, fade out finished after 299.8 ms, level ready 300.0 ms after Continue, HUD and subtitles restored, fade in finished after 500.1 ms, no loading screen; level 3 loading path (Continue in the Stats.Next frame): loading screen shown, progress 0.25 / 0.50 / 0.75 / 1.00 over frames 63126-63129, level ready 582.6 ms after Continue, loading screen hidden, fade in finished after 526.9 ms. Rendered -game at 1280 x 720 (task-318-04, Saved/PreloadNextLevel/, contact_sheet.png, run.log): level 2 fade path seen: cards screen darkening over 4 shots (mean luma 40, 33, 25, 17 = black), no loading screen, fade out 351 ms, switch 351 ms after Continue; the fade in was not seen (the first frame after the switch took about 0.5 s, so the next shot is already at full brightness; fade in logged 684 ms). Level 3 loading path seen: WBP_Loading with the orange bar at 0.25 then 0.75 (0.5 and 1.0 never drawn: the terrain step and the switch fall in single frames), then black, then a dimmed level (about 80 %), then full; level ready 670 ms after Continue, fade in logged 705 ms. Gotcha seen: at Continue the cards screen already shows its slots as "Sold. No refunds." under the fade out (shop closed before the fade). No automation test (owner rule).

## Relations
- [[sys-run-transition]] — parent: the transition these paths finish.
- [[sys-run-transition-preload]] — the preparation whose readiness picks the path.
- [[sys-hud-bindings]] — the controller that draws the overlay and swaps the widgets.
- [[intermission]] — the fade and loading behaviour this implements.
- [[task-318-02-fade-or-loading]] — the task that built it.
