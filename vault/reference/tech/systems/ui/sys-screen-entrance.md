---
id: sys-screen-entrance
description: "Shared entrance of the end screens (run-lost, stats): URRScreenEntrance, parts by name, timings settings, skip, desaturation, rr.Debug.PressKey; open to reuse it."
parent: systems-ui-index
kind: doc
---
# Screen entrance

- responsibility: the appearance of [[death-screen]] (Appearance Q2) as one effect shared by every end screen that opts in: backdrop 0 to 1 with the level desaturated, title drop with a short shake, frame slide up with fade, buttons last, skip on a key or click (task-547, commit c7245422; [[intermission]] stats screen and its level 4 variant since then).
- main_classes: `UI/RRScreenEntrance.h/.cpp`: `URRScreenEntrance` (UObject owned by the screen widget), `FRRScreenEntranceParts` (widget names), `URRScreenEntranceSettings` (plain config object). Users: `URRRunLostWidget` ([[sys-run-lost]]) and `URRIntermissionWidget` ([[sys-run-transition]]), each with an `EntranceParts` class default set in its constructor.
- entry_points: the owner widget creates it in `NativeOnInitialized` and calls `Setup(this, EntranceParts)` (finds the parts by name in the widget tree, variables or not, and keeps their rest render transform and opacity), `Start()` right after its Blueprint fill event (OnRunLostShown, OnRecapShown), `Tick(InDeltaTime)` from `NativeTick` (real time, plays while paused), `Restore()` in `NativeDestruct`. `Skip()` jumps to the end state; `OnFinished` (native delegate) fires once at the end state, played or skipped (run-lost: `FinishAppearance`, focus on Retry). Intermission adds `SkipEntrance()` and `IsEntrancePlaying()` (BlueprintCallable).
- behaviour: `Apply(T)`: Backdrop opacity and level desaturation over [0, BackdropEnd]; TitleGroup opacity over [BackdropEnd, TitleEnd], Title render scale TitleStartScale to 1, then shake X = sin(T x ShakeSpeed) x ShakeAmplitude fading to 0 at ShakeEnd; FrameGroup opacity and slide up from FrameSlide over [TitleEnd, FrameEnd]; Buttons opacity over [FrameEnd, ButtonsEnd]; end state at TotalSeconds. Skip: a key or mouse button of the owning player (`WasInputKeyJustPressed(AnyKey)`) or a click on the SkipButton part; a screen without one gets an invisible full-screen `EntranceSkipButton` on its root canvas (Z 1000). Buttons are HitTestInvisible until the end state, so a skip never presses one. Desaturation: view target camera post process saturation W x (1 - amount), moved here from `URRRunLostWidget::SetLevelDesaturation` (which now forwards); the controller gets `bShouldPerformFullTickWhenPaused` during the entrance and two frames after (so the paused camera applies it), then its previous value; `Restore` puts saturation and flag back.
- data: Config/DefaultGame.ini `[/Script/RogueRigs.RRScreenEntranceSettings]` (none written; code defaults, death-screen values, provisional): BackdropEnd 0.6, TitleEnd 0.85, FrameEnd 1.15, ButtonsEnd 1.35, TotalSeconds 1.4, TitleStartScale 1.4, ShakeAmplitude 8, ShakeSpeed 90, ShakeEnd 1.0, FrameSlide 40, LevelDesaturation 1.
- debug: `UI/RRScreenEntranceDebugCommands.cpp`: `rr.Debug.PressKey <Key>` (simulated press on the first player controller, released 0.1 s later) checks the key skip in a -game capture.
- depends_on: [[sys-button-hover]] (the same buttons, render scale untouched here).
- implements: [[death-screen]], [[intermission]]
- gotchas: in a fresh -game run the first frame after a skip can miss the button plate (first draw of its material, shader hitch) and the desaturation lands one frame later. Leaving the stats screen restores the level colour behind the shop screen. Verified task-547: Saved/Captures/t547_sheet.png.

## Relations
- [[systems-ui-index]] — parent index.
- [[death-screen]] — the timings it plays.
- [[intermission]] — the stats screen that shares it.
- [[sys-run-lost]] — run-lost widget, first user.
- [[sys-run-transition]] — stats screen widget base, second user.
- [[wbp-run-lost]] — Blueprint whose old entrance graph it replaces.
- [[wbp-intermission]] — Blueprint it drives without a graph.
- [[sys-button-hover]] — other shared screen-button effect.
