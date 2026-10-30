---
id: wbp-loading
description: "WBP_Loading, opaque loading screen between levels (DROPPING IN, Level N, stage line) from English screen texts, bound to the C++ loading events."
parent: blueprints-ui-index
kind: doc
---
# WBP_Loading

- asset_path: /Game/RogueRigs/UI/WBP_Loading
- parent_class: URRLoadingScreenWidget (/Script/RogueRigs.RRLoadingScreenWidget, [[sys-run-transition]])
- purpose: hides the map being built during a level change: opaque dark screen, "DROPPING IN", "Level N" and a stage line, read from the validated English texts (screen_texts.json item `loading_level`) through [[sys-text-content]]. Created by the controller (class in LoadingScreenWidgetClass of [[bp-rr-player-controller]]) when loading starts, over the main menu on New Run by the menu controller (class in LoadingScreenWidgetClass of [[bp-main-menu-game-mode]], task-379), again on the new world's BeginPlay, removed on OnLevelReady.
- components: Border Backdrop (root, brush colour 0.02/0.025/0.035 alpha 1, fills the screen) > VerticalBox Column (centred both ways) > TitleText "DROPPING IN" (Roboto Bold 64, white, outline 2), LevelText "Level 1" (Bold 36, orange 1/0.35/0.1, outline 2, top padding 16), StageText (Bold 22, grey 0.7, outline 2, top padding 16, initial "Surveying the terrain"), SizeBox LoadingBarBox (600 x 24, slot top padding 32, centred) > ProgressBar LoadingBar (variable, fill 1/0.35/0.1 as the AlertBar of [[wbp-alert-objectives]], default engine background, Percent 0), Image LoadingImage (variable, Collapsed, no texture, slot top padding 24, centred; reserved for the later loading image), Image LoadingAnimImage (variable, task-379-03, after LoadingImage: brush `T_Icon_Ore` 128 x 128, Collapsed by default, slot top padding 32, HAlign Center, render pivot 0.5/0.5); LoadingBarBox is now a variable. The three texts are variables, centre-justified and aligned. Same font and colour language as [[wbp-alert-objectives]]. The crystal spin is tick-driven, no UMG widget animation asset (owner accepted 2026-10-30). Variables SpinAngle (float), bSpinning (bool).
- exposed_defaults: none.
- events: both events first call the function LoadScreenTexts (gets `URRTextContentSubsystem` from the widget world, `GetScreenText("loading_level")`, stores the struct in variable ScreenTexts and the result in bHasTexts). OnLoadingShown(LevelIndex, LayoutName): if bHasTexts, TitleText = Title and LevelText = Labels[0] with "{N}" replaced by LevelIndex (literal string Replace on the converted text); otherwise TitleText keeps its default and LevelText = "Level " + LevelIndex. OnLoadingStageChanged(Stage): if bHasTexts, switch on ERRRunLoadingStage: WaitingForLayout Lines[0], OpeningMap Lines[1], PreparingLevel Lines[2]; None leaves the text unchanged; without texts the tree default stays. OnLoadingProgressChanged(Progress) (task-318-03): sets LoadingBar percent to Progress (the C++ event, also called once at ShowLoading; progress steps 0.25 to 1 in the level preparation, 0 on the map-travel fallback, see [[sys-run-transition-fade]]).
- logic_in_bp: display only; the crystal spin and the bar-or-animation choice (below).
  - OnLoadingShown, after the existing logic: if `IsLoadingProgressMeasured` is false (New Run, map-travel fallback), LoadingBarBox Collapsed, LoadingAnimImage HitTestInvisible, bSpinning true; else the bar is kept and the image Collapsed ([[sys-run-transition-fade]]).
  - EventTick: if bSpinning, SpinAngle = (SpinAngle + DeltaTime * 180) % 360, then SetRenderTransformAngle(LoadingAnimImage, SpinAngle): one turn per 2 s, looping. EventDestruct sets bSpinning false.
  - Shown for New Run (menu world) and between levels on the map-travel fallback. Q5 (task-379, owner accepted): the game thread is blocked during the map load, so the image is frozen then; the spin resumes in the arena hold.
  - Unverified: the owner saw the crystal spin and said it takes a while to start. What in the code could delay the first turn: it starts only in OnLoadingShown; the first Slate paint wait (`LoadingPaintWaitMaxFrames`) before the travel; the game thread blocked during OpenLevel; the Q5 freeze. Not measured, lag not confirmed.
  - Lead only (not implemented, not a decision): an async package preload of the arena map while the loading screen is up could shorten the blocking OpenLevel ([[sys-run-transition]]).
- assets: none.
- owner_agent: agent-bp
- verified: task-206-05: tree read back, graph read back, compiled with warnings as errors, saved. task-210-04: tree defaults read back (DROPPING IN, Level 1, Surveying the terrain), graph wiring read back, compiled with warnings as errors, saved. task-318-03: tree and slot values read back (bar 600 x 24, fill orange, image Collapsed, brush no image), graph read back, compiled with warnings as errors, saved. task-318-04: seen on screen in a rendered -game run (Saved/PreloadNextLevel/t318_loading_00.png, _01.png): dark backdrop, "DROPPING IN", orange "Level 3", stage line "Prepping the level" (runtime text lookup works), LoadingBar orange fill at 0.25 then 0.75; 0.5 and 1.0 not drawn (single-frame steps); LoadingImage not visible (collapsed). task-379-03 (commits bdc75c51, d3b6e4ac): LoadingAnimImage tree and tick-driven spin added, LoadingImage untouched, compiled with warnings as errors, saved. task-379-04: capture closed on the owner's word, no capture run; owner saw the spinning crystal.

## Relations
- [[blueprints-ui-index]] — parent index.
- [[sys-text-content]] — the screen text source.
- [[sys-run-transition]] — the C++ base class, loading events and stages.
- [[sys-run-transition-fade]] — the progress API the bar follows.
- [[bp-main-menu-game-mode]] — creates it over the menu on New Run.
- [[task-379-new-run-loading-screen]] — the task that added the animation and the New Run use.
- [[bp-rr-player-controller]] — creates it through LoadingScreenWidgetClass.
- [[wbp-alert-objectives]] — font and colour reference.
