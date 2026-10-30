---
id: task-379-new-run-loading-screen
description: "Global task: Main menu New Run shows the level-change loading screen before the map opens and until level 1 is ready, instead of a direct OpenLevel."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Main menu New Run: show the loading screen

- request: "Il faudrait l'écran de chargement aussi lorsqu'on fait 'New run' depuis le main menu." (owner, 2026-10-30, verbatim, French, relayed by the main session; no file or image attached). One logical point, one global. Today URRMainMenuWidget::StartNewRunInWorld (Source/RogueRigs/Menu/RRMainMenuWidget.cpp) calls URRRunSubsystem::ResetRun then UGameplayStatics::OpenLevel(URRRunSettings::NewRunMapName) directly: no loading screen, the menu freezes during the load, and level 1 starts with no spawner, run-flow or input hold. Between levels the loading screen already exists (URRRunTransitionSubsystem phase Loading, OnLoadingStarted, paint wait before the travel, PostLoadMapWithWorld hold until the spawner prewarm, OnLevelReady, fade in; ARRPlayerController shows WBP_Loading through LoadingScreenWidgetClass and re-shows it in the new world while IsLevelLoading). Not in scope: restart after death or end screens (RestartLevel), the editor and PIE start on L_ArenaM1 (no menu, unchanged), measured progress for the bar. Questions Q1 to Q3 below, each with a default.
- goal: Pressing New Run in the main menu (or rr.Menu.NewRun) shows WBP_Loading ("DROPPING IN", "Level 1") over the menu before the map opens, keeps it in the arena world until level 1 is ready, then fades in, exactly like the map-travel loading between levels.
- acceptance: New Run goes through a URRRunTransitionSubsystem entry point (ResetRun, phase Loading for level 1, OnLoadingStarted(1, level 1 layout), the existing paint wait, then OpenLevel(NewRunMapName)); the menu world shows WBP_Loading (class from a new EditDefaultsOnly LoadingScreenWidgetClass on ARRMainMenuGameMode, set to WBP_Loading_C in BP_MainMenuGameMode) and it is painted at least one frame before the travel (log: first loading paint frame < opening frame); in L_ArenaM1 the controller shows it again while IsLevelLoading, HUD and subtitles hidden, spawner, run flow and gameplay input held until the prewarm (or LoadingHoldTimeoutSeconds), then OnLevelReady removes it and the 0.5 s fade in plays (Q1); bar empty on this path (Q2); run starts at level 1/4 on arena_m1_fine with ore bank 0 as today; a second press still ignored; rr.Menu.NewRun takes the same path; PIE or -game directly on L_ArenaM1 unchanged (no loading screen); between-level transitions unchanged; existing tests the change breaks updated, no new test; capture checklist C1 to C4 seen (Q3); docs updated; gdd main-menu updated; lint clean; committed by path.
- gdd_check: read 2026-10-30: [[main-menu]] (provisional, owner 2026-10-28: "New Run: resets the run and opens L_ArenaM1 at level 1", no loading screen mentioned; font and colours already reference [[wbp-loading]]), [[intermission]] (provisional: loading screen with a progress bar until the level is ready, then fade in; the image slot open, not needed here). Tech: [[sys-main-menu]] (StartNewRun / StartNewRunInWorld, menu game mode and controller, rr.Menu.NewRun), [[sys-run]] (ResetRun, NewRunMapName, BeginRun on the first miner BeginPlay when no run is in progress), [[sys-run-transition]] (phase Loading, paint wait LoadingPaintWaitMaxFrames 5, PostLoadMapWithWorld hold, LoadingHoldTimeoutSeconds 10, controller re-show while IsLevelLoading reads GetIntermissionRecap().NextLayoutName), [[sys-run-transition-fade]] (map-travel fallback: progress 0, fade in 0.5 s after OnLevelReady), [[wbp-loading]] (texts from screen_texts.json loading_level, "Level {N}"), [[bp-main-menu-game-mode]] (MainMenuWidgetClass only). Code checked: RRMainMenuWidget.cpp lines 34-54 direct OpenLevel; RRPlayerController.cpp ShowLoadingScreen and the BeginPlay re-show. Conflict: none (the gdd main-menu gains the loading screen, edit below). Open values needed: fade in (Q1), bar behaviour (Q2); the loading image stays open and unused. Scope: M2 like task-338 and task-365. Overlap: none in progress (task-314 todo, unrelated).
- confirmed_by_owner: no (plan and questions handed back 2026-10-30)
- milestone: M2
- next_step: task-379-01-new-run-loading-code
- started: open
- waiting_for: owner confirmation of the plan
- result: open

## Questions
Asked 2026-10-30, each with a default; open until the owner answers.
- Q1 À la fin du chargement (niveau 1 prêt), même fondu au noir qu'entre les niveaux : l'écran de chargement disparaît et le niveau apparaît en 0,5 s depuis le noir. Autre choix : coupe directe sans fondu. Default: fondu 0,5 s.
- Q2 La barre de progression : sur ce chemin (chargement de carte), Unreal ne mesure pas l'avancement, donc la barre reste vide comme aujourd'hui sur le chargement de carte entre niveaux ; seules les lignes d'étape changent ("Opening map", puis "Prepping the level"). Autre choix (travail en plus) : des paliers grossiers (0,5 carte chargée, 1 niveau prêt), aussi entre les niveaux. Default: barre vide, comme aujourd'hui.
- Q3 Une sous-tâche capture en -game vérifie à l'image : l'écran de chargement par-dessus le menu avant l'ouverture de la carte, puis dans l'arène, puis le niveau 1 avec le HUD ; résultats envoyés à toi. Autre choix : pas de capture, tu regardes toi-même. Default: oui, capture.

## Subtasks
1. task-379-01-new-run-loading-code (vault/reference/tasks/task-379-new-run-loading-screen/task-379-01-new-run-loading-code.md) — cpp: New Run goes through the transition Loading phase: loading screen in the menu world before OpenLevel, hold and loading screen in the arena until level 1 is ready, fade in
2. task-379-02-menu-game-mode-loading-class (vault/reference/tasks/task-379-new-run-loading-screen/task-379-02-menu-game-mode-loading-class.md) — bp: BP_MainMenuGameMode: set LoadingScreenWidgetClass to WBP_Loading
3. task-379-03-new-run-loading-capture (vault/reference/tasks/task-379-new-run-loading-screen/task-379-03-new-run-loading-capture.md) — capture: Rendered -game check from the main menu: loading screen over the menu, in the arena, then level 1 with the HUD
4. task-379-04-doc-new-run-loading (vault/reference/tasks/task-379-new-run-loading-screen/task-379-04-doc-new-run-loading.md) — doc: Update the menu, run, transition and menu game mode docs for the New Run loading path

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-379-01-new-run-loading-code]] — step 1, cpp.
- [[task-379-02-menu-game-mode-loading-class]] — step 2, bp.
- [[task-379-03-new-run-loading-capture]] — step 3, capture.
- [[task-379-04-doc-new-run-loading]] — step 4, doc.
- [[main-menu]] — the gdd screen whose New Run gains the loading screen.
- [[sys-run-transition]] — the loading phase New Run now goes through.
- [[sys-main-menu]] — the menu C++ whose New Run is rerouted.
