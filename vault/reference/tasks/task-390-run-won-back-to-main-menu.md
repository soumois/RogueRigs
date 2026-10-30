---
id: task-390-run-won-back-to-main-menu
description: "Global task: After the level 4 extraction, the run-won end screen button returns to the main menu instead of restarting a run."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Run won: back to the main menu

- request: Owner, 2026-10-30, after testing the packaged build (Saved/Packaged/Windows/RogueRigs.exe), relayed verbatim by the main session (French), four points: "La lueur du coffre bonus reste entre les niveaux" / "Les particules de robot doivent aussi être retirées entre les niveaux (je pouvais voir des particules des robots tués au précédent niveau car j'ai passé rapidement à l'autre niveau)" / "Le dernier niveau doit ramener vers le main menu" / "Pour le dernier écran pour les ore sécurisés (ça n'est pas un autre mot que ore ?), il ne comptait pas ceux du level en cours." A screenshot of the arena was attached (aim chevrons visible, no issue there). This task is point 3 only: "Le dernier niveau doit ramener vers le main menu". Reading (Planner): today, after the level 4 extraction, the stats screen leads to the run-won end screen (CLAIM STRIPPED, Ore secured, button "Play again" which restarts at level 1); the owner wants the end of the last level to bring the player back to the main menu. How (which screen, button label, loss screen, PIE) is asked in Q3 and Q4. GDD conflict: [[main-menu]] says "Not in scope: a back to menu button on end screens"; changed by this task on confirmation.
- goal: On a won run (level 4 extracted), the run-won end screen shows a "Main Menu" button; pressing it resets the run (ore 0) and opens L_MainMenu; New Run from there starts level 1. The run-lost screen keeps "Play again".
- acceptance: cpp: BlueprintCallable return-to-menu entry (run reset, then OpenLevel of a new URRRunSettings menu map setting, default /Game/RogueRigs/Maps/L_MainMenu) and a debug command to trigger it; bp: WBP_AlertObjectives run-won end screen button reads "Main Menu" and calls it, the loss screen and earlier-level success keep "Play again" and RestartLevel; compiled with warnings as errors; build passes; capture C1 to C4 seen; gdd main-menu and run-structure updated (edits applied on confirmation); docs updated by the doc subtask; lint clean; committed by path.
- gdd_check: Read 2026-10-30: [[run-structure]] (approved: win at level 4 = stats screen then run-won screen; lose = run-lost screen, restart at level 1): extended, the run-won screen button returns to the menu (edit). [[main-menu]] (provisional: L_MainMenu, New Run resets the run; "Not in scope: a back to menu button on end screens or in pause"): conflict with the owner words, changed by this task (edit: run won only, Q3, Q4). [[ore-economy]]: all ore reset at run end, kept (ResetRun does it). Tech read: [[wbp-alert-objectives]] (EndScreen, RestartButton "Play again" calls RunFlow RestartLevel; ShowRunEnd uses IsRunWon for the end_run_won texts), [[sys-run-transition]] (NextLevel on the last level calls ShowRunWonEndScreen), [[sys-main-menu]] (L_MainMenu, menu game mode, NewRunMapName; packaged MapsToCook lists L_MainMenu and L_ArenaM1). Open values: none after Q3, Q4 (label "Main Menu" English, hardcoded like the menu labels). Scope M2. Capture per Q1 of task-388. task-391 may change the same widget afterwards: it runs after this task. Repo state 2026-10-30: uncommitted changes not made by this task in Source/RogueRigs/Player/RRPlayerController.cpp/.h (RestoreGameInputMode, packaged input fix), Config/DefaultGame.ini and Config/DefaultEditor.ini: leave them untouched and never commit them with this task (commit by path). task-387 (art, wait-answers) shares no file.
- confirmed_by_owner: no (plan and questions handed back 2026-10-30)
- milestone: M2
- next_step: task-390-01-return-to-menu-code
- started: open
- waiting_for: owner confirmation of the plan
- result: open

## Questions
Asked 2026-10-30, each with a default; open until the owner answers.
- Q3 Comment ramener au menu : l'écran de victoire (CLAIM STRIPPED, Ore secured) reste après l'écran de stats du niveau 4 ; son bouton « Play again » devient « Main Menu » et ouvre le menu principal (run remis à zéro, minerai à 0). Autre choix : le bouton de l'écran de stats du niveau 4 ouvre directement le menu principal, sans écran de victoire. Default: bouton « Main Menu » sur l'écran de victoire.
- Q4 Le reste ne change pas : l'écran de défaite garde « Play again » (recommence au niveau 1) ; le menu s'ouvre directement, sans écran de chargement ni fondu (la carte du menu est vide) ; même comportement dans le jeu packagé, en -game et dans l'éditeur (PIE). Default: oui.

## Subtasks
1. task-390-01-return-to-menu-code (vault/reference/tasks/task-390-run-won-back-to-main-menu/task-390-01-return-to-menu-code.md) — cpp: BlueprintCallable return to the main menu: run reset then L_MainMenu, menu map setting, debug command
2. task-390-02-end-screen-menu-button (vault/reference/tasks/task-390-run-won-back-to-main-menu/task-390-02-end-screen-menu-button.md) — bp: WBP_AlertObjectives: run-won end screen button "Main Menu" calls the return-to-menu entry
3. task-390-03-menu-return-capture (vault/reference/tasks/task-390-run-won-back-to-main-menu/task-390-03-menu-return-capture.md) — capture: -game frames: run-won screen with Main Menu, then the main menu, then New Run
4. task-390-04-doc-menu-return (vault/reference/tasks/task-390-run-won-back-to-main-menu/task-390-04-doc-menu-return.md) — doc: update the run flow, main menu, run and end screen docs with the return to the menu

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-390-01-return-to-menu-code]] — step 1, cpp.
- [[task-390-02-end-screen-menu-button]] — step 2, bp.
- [[task-390-03-menu-return-capture]] — step 3, capture.
- [[task-390-04-doc-menu-return]] — step 4, doc.
- [[main-menu]] — the menu spec changed: run won returns to it.
- [[run-structure]] — the run end changed here.
- [[wbp-alert-objectives]] — the end screen widget changed here.
- [[task-391-final-screens-count-level-ore]] — next task, same end screen widget.
