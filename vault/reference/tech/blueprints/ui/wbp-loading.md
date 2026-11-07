---
id: wbp-loading
description: "WBP_Loading, loading screen between levels and on New Run: full-screen illustration, riveted frame with DROPPING IN, Level N, stage line, bar or spinner."
parent: blueprints-ui-index
kind: doc
---
# WBP_Loading

- asset_path: /Game/RogueRigs/UI/WBP_Loading
- parent_class: URRLoadingScreenWidget (/Script/RogueRigs.RRLoadingScreenWidget, [[sys-run-transition]])
- purpose: hides the map being built during a level change: full-screen illustration with a riveted frame bottom left holding "DROPPING IN", "Level N" and a stage line, read from the validated English texts (screen_texts.json item `loading_level`) through [[sys-text-content]]. Created by the controller (class in LoadingScreenWidgetClass of [[bp-rr-player-controller]]) when loading starts, over the main menu on New Run by the menu controller (class in LoadingScreenWidgetClass of [[bp-main-menu-game-mode]], task-379), again on the new world's BeginPlay, removed on OnLevelReady.
- components (task-488-02, commits 5a6293ae, ef55572e; look of [[intermission]], owner 2026-11-06, provisional): CanvasPanel Root, direct children bottom to top:
  - Backdrop (Border, brush colour 0.02/0.025/0.035 alpha 1, now empty): anchors (0,0)-(1,1), offsets 0; dark base under the image.
  - BackgroundScale (ScaleBox, Stretch ScaleToFill, HitTestInvisible): anchors (0,0)-(1,1), offsets 0; content slot HAlign Center, VAlign Top (wider than 16:9 crops the bottom, not the shuttle; 16:10 crops the sides). Holds LoadingImage (Image, variable, the slot reserved since task-318, now used: brush T_LoadingBackground 1672 x 941, HitTestInvisible).
  - BottomGradient (Image, T_Loading_BottomGradient 8 x 256, transparent to dark violet-black alpha 0.92, smoothstep; HitTestInvisible): anchors (0,0.5)-(1,1), offsets 0.
  - Frame (Border, HitTestInvisible, brush T_LevelEnd_Frame drawn as Box, margins L/R 0.0955, T/B 0.1878 as [[wbp-intermission]]; padding 56/44/72/52): anchors (0,1)-(0,1), alignment (0,1), offsets 48, -48, auto size (its size comes from the texts). Child Column (VerticalBox, Fill/Fill) in order: TitleText "DROPPING IN" (Roboto Bold 52, white, outline 2, left), LevelText (Bold 32, orange 1/0.35/0.1, outline 2, top padding 8, left), StageText (Bold 22, grey 0.7, outline 2, top padding 8, left, initial "Surveying the terrain"), LoadingBarBox (SizeBox, variable, height 45, no width override, slot HAlign Fill, top padding 24: as wide as the widest text) > BarTrack (Overlay) > LoadingBar (ProgressBar, variable, padding 13/10/13/10; background and fill brushes drawn as Image with no texture: background tint 0.012/0.011/0.018, fill white tinted by FillColorAndOpacity 1/0.35/0.1; no engine bar images) and BarFrame (Image, T_HUD_HealthStrip drawn as Box, margins 0.028/0.22: riveted strip with a transparent window), LoadingAnimImage (variable, T_Icon_Ore 128 x 128, Collapsed by default, slot HAlign Center, padding top 16 bottom 8, render pivot 0.5/0.5).
  - The three texts are variables. Variables SpinAngle (float), bSpinning (bool). The crystal spin is tick-driven, no UMG animation asset (owner accepted 2026-10-30).
  - Before task-488: Border Backdrop root > centred VerticalBox (title 64, level 36, stage 22, centred), engine-look bar 600 x 24, LoadingImage Collapsed with no texture.
  - Owner retouch points: Frame slot offsets, Frame padding, font sizes, LoadingBarBox height, BarTrack paddings, BottomGradient top anchor.
- exposed_defaults: none.
- events: both events first call the function LoadScreenTexts (gets `URRTextContentSubsystem` from the widget world, `GetScreenText("loading_level")`, stores the struct in variable ScreenTexts and the result in bHasTexts). OnLoadingShown(LevelIndex, LayoutName): if bHasTexts, TitleText = Title and LevelText = Labels[0] with "{N}" replaced by LevelIndex (literal string Replace on the converted text); otherwise TitleText keeps its default and LevelText = "Level " + LevelIndex. OnLoadingStageChanged(Stage): if bHasTexts, switch on ERRRunLoadingStage: WaitingForLayout Lines[0], OpeningMap Lines[1], PreparingLevel Lines[2]; None leaves the text unchanged; without texts the tree default stays. OnLoadingProgressChanged(Progress) (task-318-03): sets LoadingBar percent to Progress (the C++ event, also called once at ShowLoading; progress steps 0.25 to 1 in the level preparation, 0 on the map-travel fallback, see [[sys-run-transition-fade]]).
- logic_in_bp: display only; the crystal spin and the bar-or-animation choice (below).
  - OnLoadingShown, after the existing logic: if `IsLoadingProgressMeasured` is false (New Run, map-travel fallback), LoadingBarBox Collapsed, LoadingAnimImage HitTestInvisible, bSpinning true; else the bar is kept and the image Collapsed ([[sys-run-transition-fade]]).
  - EventTick: if bSpinning, SpinAngle = (SpinAngle + DeltaTime * 180) % 360, then SetRenderTransformAngle(LoadingAnimImage, SpinAngle): one turn per 2 s, looping. EventDestruct sets bSpinning false.
  - Shown for New Run (menu world) and between levels on the map-travel fallback. Q5 (task-379, owner accepted): the game thread is blocked during the map load, so the image is frozen then; the spin resumes in the arena hold.
  - Unverified: the owner saw the crystal spin and said it takes a while to start. What in the code could delay the first turn: it starts only in OnLoadingShown; the first Slate paint wait (`LoadingPaintWaitMaxFrames`) before the travel; the game thread blocked during OpenLevel; the Q5 freeze. Not measured, lag not confirmed.
  - Lead only (not implemented, not a decision): an async package preload of the arena map while the loading screen is up could shorten the blocking OpenLevel ([[sys-run-transition]]).
- assets: /Game/RogueRigs/UI/T_LoadingBackground (task-488-01, source `vault/assets/loading-screen-background.png`, [[art-prompts-loading-screen]]; UI group, no mipmaps, sRGB, never stream); /Game/RogueRigs/UI/Loading/T_Loading_BottomGradient (source `Art/UI/Loading/T_Loading_BottomGradient.png`, made by `Art/UI/Loading/make_gradient.js`; UI group, no mipmaps, clamp); reused T_LevelEnd_Frame, T_HUD_HealthStrip, T_Icon_Ore.
- owner_agent: agent-bp
- verified: task-488 (2026-11-07): CompileWidgetBlueprint true, compiled with warnings as errors with no log warning, saved; -game captures of both cases (New Run spinner, between-levels bar) at 1920 x 1080, 1280 x 720, 1920 x 1200 (Saved/Captures/t488_r2_<res>/) and 2560 x 1080 after the top-align fix (t488_r3_2560x1080/), sheet with the before frame Saved/Captures/t488_final_sheet.png (not committed): image fills the screen, frame bottom left with readable texts, orange bar in the riveted track, spinner in the frame, nothing clipped. Earlier: task-206-05: tree read back, graph read back, compiled with warnings as errors, saved. task-210-04: tree defaults read back (DROPPING IN, Level 1, Surveying the terrain), graph wiring read back, compiled with warnings as errors, saved. task-318-03: tree and slot values read back (bar 600 x 24, fill orange, image Collapsed, brush no image), graph read back, compiled with warnings as errors, saved. task-318-04: seen on screen in a rendered -game run (Saved/PreloadNextLevel/t318_loading_00.png, _01.png): dark backdrop, "DROPPING IN", orange "Level 3", stage line "Prepping the level" (runtime text lookup works), LoadingBar orange fill at 0.25 then 0.75; 0.5 and 1.0 not drawn (single-frame steps); LoadingImage not visible (collapsed). task-379-03 (commits bdc75c51, d3b6e4ac): LoadingAnimImage tree and tick-driven spin added, LoadingImage untouched, compiled with warnings as errors, saved. task-379-04: capture closed on the owner's word, no capture run; owner saw the spinning crystal.

## Relations
- [[blueprints-ui-index]] — parent index.
- [[sys-text-content]] — the screen text source.
- [[sys-run-transition]] — the C++ base class, loading events and stages.
- [[sys-run-transition-fade]] — the progress API the bar follows.
- [[bp-main-menu-game-mode]] — creates it over the menu on New Run.
- [[task-379-new-run-loading-screen]] — the task that added the animation and the New Run use.
- [[bp-rr-player-controller]] — creates it through LoadingScreenWidgetClass.
- [[wbp-alert-objectives]] — font and colour reference.
- [[intermission]] — the loading screen look (task-488).
- [[art-prompts-loading-screen]] — the illustration and its prompts.
- [[wbp-intermission]] — the stats screen frame it reuses.
