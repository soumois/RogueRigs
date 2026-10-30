---
id: task-379-new-run-loading-screen
description: "Global task: Main menu New Run shows the loading screen until level 1 is ready; an animated image replaces the unmeasured progress bar."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Main menu New Run: show the loading screen

- request: "Il faudrait l'écran de chargement aussi lorsqu'on fait 'New run' depuis le main menu." (owner, 2026-10-30, verbatim, French, relayed by the main session; no file or image attached). One logical point, one global. Today URRMainMenuWidget::StartNewRunInWorld (Source/RogueRigs/Menu/RRMainMenuWidget.cpp) calls URRRunSubsystem::ResetRun then UGameplayStatics::OpenLevel(URRRunSettings::NewRunMapName) directly: no loading screen, the menu freezes during the load, and level 1 starts with no spawner, run-flow or input hold. Between levels the loading screen already exists (URRRunTransitionSubsystem phase Loading, OnLoadingStarted, paint wait before the travel, PostLoadMapWithWorld hold until the spawner prewarm, OnLevelReady, fade in; ARRPlayerController shows WBP_Loading through LoadingScreenWidgetClass and re-shows it in the new world while IsLevelLoading). Owner answer 2026-10-30 to the first plan (Q1 fade 0.5 s, Q2 empty bar, Q3 capture), relayed by the main session, verbatim: "Q2 s'il n'y a pas de barre de progression, il faut alors une animation d'une image ou autre. Ok pour le reste". So Q1 and Q3 defaults accepted; Q2 replaced: when the progress cannot be measured (a map load), WBP_Loading hides the bar and shows a looping animated image; this is the same widget, so the between-levels map-travel fallback gets it too; the measured in-world preparation path keeps the bar. Not in scope: restart after death or end screens (RestartLevel), the editor and PIE start on L_ArenaM1 (unchanged), measured progress for map loads. New questions Q4 and Q5 below, each with a default. Owner answer 2026-10-30 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q4, Q5 accepted.
- goal: Pressing New Run in the main menu (or rr.Menu.NewRun) shows WBP_Loading ("DROPPING IN", "Level 1") over the menu before the map opens, keeps it in the arena world until level 1 is ready, then fades in; on every loading screen whose progress is not measured, an animated image replaces the bar.
- acceptance: New Run goes through a URRRunTransitionSubsystem entry point (ResetRun, phase Loading for level 1, OnLoadingStarted(1, level 1 layout), the existing paint wait, then OpenLevel(NewRunMapName)); the menu world shows WBP_Loading (class from a new EditDefaultsOnly LoadingScreenWidgetClass on ARRMainMenuGameMode, set to WBP_Loading_C in BP_MainMenuGameMode) and it is painted at least one frame before the travel (log: first loading paint frame < opening frame); in L_ArenaM1 the controller shows it again while IsLevelLoading, HUD and subtitles hidden, spawner, run flow and gameplay input held until the prewarm (or LoadingHoldTimeoutSeconds), then OnLevelReady removes it and the 0.5 s fade in plays (Q1); URRLoadingScreenWidget tells Blueprint whether the progress is measured; WBP_Loading with unmeasured progress (New Run, between-levels map-travel fallback) hides the bar and plays the looping image animation (Q2 answer, Q4), with measured progress (in-world preparation between levels) shows the bar as today; behaviour during the blocking load per Q5; run starts at level 1/4 on arena_m1_fine with ore bank 0 as today; a second press still ignored; rr.Menu.NewRun takes the same path; PIE or -game directly on L_ArenaM1 unchanged; existing tests the change breaks updated, no new test; capture checklist C1 to C5 seen (Q3); docs updated; gdd main-menu and intermission updated; lint clean; committed by path.
- gdd_check: read 2026-10-30: [[main-menu]] (provisional, owner 2026-10-28: "New Run: resets the run and opens L_ArenaM1 at level 1", no loading screen; updated 2026-10-30 with the decided Q1 to Q3), [[intermission]] (provisional: loading screen with a progress bar until the level is ready, then fade in; image slot kept for a later loading image, image open; updated 2026-10-30: unmeasured progress shows a looping animation, image and animation open (Q4), blocking-load behaviour open (Q5)). Tech: [[sys-main-menu]] (StartNewRun / StartNewRunInWorld, menu game mode and controller, rr.Menu.NewRun), [[sys-run]] (ResetRun, NewRunMapName, BeginRun on the first miner BeginPlay), [[sys-run-transition]] (phase Loading, paint wait LoadingPaintWaitMaxFrames 5, PostLoadMapWithWorld hold, LoadingHoldTimeoutSeconds 10, controller re-show reads GetIntermissionRecap().NextLayoutName, empty on New Run; no MoviePlayer: map load 2.3 to 3.3 s measured between levels, the game thread is blocked then, so no UMG animation moves during it), [[sys-run-transition-fade]] (map-travel fallback: progress 0; fade in 0.5 s after OnLevelReady), [[wbp-loading]] (LoadingBarBox, LoadingImage reserved for the later loading image, kept free), [[wbp-ore-counter]] (existing T_Icon_Ore, 256 x 256 RGBA, the Q4 default), [[bp-main-menu-game-mode]]. Code checked: RRMainMenuWidget.cpp lines 34-54 direct OpenLevel; RRLoadingScreenWidget.h progress API (GetLoadingProgress, OnLoadingProgressChanged), no measured flag; RRPlayerController.cpp ShowLoadingScreen and BeginPlay re-show. Conflict: none. Open values needed: the image and animation (Q4, art: the vault has no loading animation or image), the blocking-load freeze (Q5). Scope: M2 like task-338 and task-365. Overlap: none in progress (task-314 todo, unrelated).
- confirmed_by_owner: yes (2026-10-30, "ok")
- milestone: M2
- next_step: task-379-03-loading-animation-widget
- started: 2026-10-30T03:09:42Z
- waiting_for: owner: stop orphan UnrealEditor-Cmd PIDs 53632 and 49888 (lock WBP_Loading.uasset, save fails); edits are unsaved in the open editor
- result: open

## Questions
Decided 2026-10-30 (owner "ok"): the defaults of Q4, Q5 are accepted; values provisional, owner 2026-10-30.
- Q1 (decided 2026-10-30, default accepted) Fondu 0,5 s depuis le noir à la fin du chargement.
- Q2 (decided 2026-10-30, owner answer replaces the default) Quand la progression n'est pas mesurée (chargement de carte : New Run, et le chargement de carte entre niveaux), la barre est cachée et une animation d'image en boucle la remplace ; la barre reste quand la progression est mesurée.
- Q3 (decided 2026-10-30, default accepted) Sous-tâche capture en -game.
- Q4 Quelle image et quelle animation : l'icône de minerai Duskite du HUD (T_Icon_Ore, existe déjà), en 128 x 128 à la place de la barre, qui tourne sur elle-même, un tour en 2 s, en boucle. Autres choix : la même icône qui pulse (grossit et rétrécit) au lieu de tourner ; ou une nouvelle image faite par l'agent art (par exemple le drone vu de dessus), ce qui ajoute une sous-tâche art et une image à valider. Default: icône de minerai qui tourne, un tour en 2 s.
- Q5 Pendant le chargement de la carte lui-même (environ 2 à 3 s), Unreal bloque le jeu : l'animation s'arrête (image figée), puis repart dans l'arène pendant la préparation du niveau. La faire tourner aussi pendant ce blocage demande l'écran de chargement du moteur (MoviePlayer), un écran séparé refait en C++ : une grosse sous-tâche cpp en plus. Default: on accepte l'image figée pendant le chargement de la carte.

## Subtasks
1. task-379-01-new-run-loading-code (vault/reference/tasks/task-379-new-run-loading-screen/task-379-01-new-run-loading-code.md) — cpp: New Run goes through the transition Loading phase (loading screen in the menu world, hold in the arena, fade in); loading widget tells Blueprint whether progress is measured
2. task-379-02-menu-game-mode-loading-class (vault/reference/tasks/task-379-new-run-loading-screen/task-379-02-menu-game-mode-loading-class.md) — bp: BP_MainMenuGameMode: set LoadingScreenWidgetClass to WBP_Loading
3. task-379-03-loading-animation-widget (vault/reference/tasks/task-379-new-run-loading-screen/task-379-03-loading-animation-widget.md) — bp: WBP_Loading: looping image animation in place of the bar when progress is not measured
4. task-379-04-new-run-loading-capture (vault/reference/tasks/task-379-new-run-loading-screen/task-379-04-new-run-loading-capture.md) — capture: Rendered -game check from the main menu: loading screen with the animation over the menu, in the arena, then level 1 with the HUD
5. task-379-05-doc-new-run-loading (vault/reference/tasks/task-379-new-run-loading-screen/task-379-05-doc-new-run-loading.md) — doc: Update the menu, run, transition, loading widget and menu game mode docs

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-379-01-new-run-loading-code]] — step 1, cpp.
- [[task-379-02-menu-game-mode-loading-class]] — step 2, bp.
- [[task-379-03-loading-animation-widget]] — step 3, bp.
- [[task-379-04-new-run-loading-capture]] — step 4, capture.
- [[task-379-05-doc-new-run-loading]] — step 5, doc.
- [[main-menu]] — the gdd screen whose New Run gains the loading screen.
- [[intermission]] — the gdd loading rule that gains the animation for unmeasured progress.
- [[sys-run-transition]] — the loading phase New Run now goes through.
- [[sys-main-menu]] — the menu C++ whose New Run is rerouted.
- [[wbp-loading]] — the loading widget that gains the animation.
