---
id: task-390-run-won-back-to-main-menu
description: "Global task: After the level 4 extraction, the run-won end screen button reads Main Menu and returns to the main menu instead of restarting a run."
parent: tasks-to-do-index
kind: task
status: todo
---
# Run won: back to the main menu

- request: Owner, 2026-10-30, after testing the packaged build (Saved/Packaged/Windows/RogueRigs.exe), relayed verbatim by the main session (French), four points: "La lueur du coffre bonus reste entre les niveaux" / "Les particules de robot doivent aussi être retirées entre les niveaux (je pouvais voir des particules des robots tués au précédent niveau car j'ai passé rapidement à l'autre niveau)" / "Le dernier niveau doit ramener vers le main menu" / "Pour le dernier écran pour les ore sécurisés (ça n'est pas un autre mot que ore ?), il ne comptait pas ceux du level en cours." A screenshot of the arena was attached (aim chevrons visible, no issue there). This task is point 3 only: "Le dernier niveau doit ramener vers le main menu". Reading (Planner): today, after the level 4 extraction, the stats screen (CLAIM EMPTIED, button "Cash out") leads to the run-won end screen (CLAIM STRIPPED, Ore secured, button "Play again", which restarts at level 1); the owner wants the end of the last level to bring the player back to the main menu. Decided by Q3, Q4 (defaults). GDD main-menu and run-structure changed (applied 2026-10-30). Owner answers 2026-10-30 (relayed by the main session, translated): Q1 NO capture subtask on any task (388 to 393): the owner tests by himself; Q2, Q3, Q4 defaults accepted (389 after 388; "Main Menu" button on the run-won screen, loss screen keeps "Play again"; menu opens directly, same behaviour in all modes); the main-menu and run-structure GDD edits approved (applied 2026-10-30); Q5, Q6 defaults stay (both screens, same number as Total Ore); Q7 changed: every on-screen "Ore" becomes "Duskite", its own task (task-392); task-391 evidence added; new point: the miner looks over-lit (task-393).
- goal: On a won run (level 4 extracted), the run-won end screen shows a "Main Menu" button; pressing it resets the run (ore 0) and opens L_MainMenu directly; New Run from there starts level 1. The run-lost screen keeps "Play again".
- acceptance: cpp: BlueprintCallable return-to-menu entry (ResetRun, then OpenLevel of a new URRRunSettings menu map setting, default /Game/RogueRigs/Maps/L_MainMenu) and a non-shipping debug command calling it; bp: WBP_AlertObjectives run-won end screen button reads "Main Menu" and calls it, loss screen and other cases keep "Play again" and RestartLevel; compiled with warnings as errors; build passes; -game log: run-won, command, run reset, L_MainMenu loaded, menu shown, New Run starts level 1/4; docs updated; lint clean; committed by path; owner checks on screen himself.
- gdd_check: Read 2026-10-30: [[run-structure]] (approved: win at level 4 = stats screen then run-won screen; lose = run-lost screen, restart at level 1) and [[main-menu]] (provisional; "Not in scope: a back to menu button on end screens"): conflict with the owner words, both nodes changed 2026-10-30 after his approval (run won returns to the menu, Q3, Q4). [[ore-economy]]: all ore reset at run end (ResetRun does it). Tech read: [[wbp-alert-objectives]] (EndScreen, RestartButton "Play again" calls RunFlow RestartLevel; ShowRunEnd uses IsRunWon for the end_run_won texts), [[sys-run-transition]] (NextLevel on the last level calls ShowRunWonEndScreen), [[sys-main-menu]] (L_MainMenu, menu game mode, NewRunMapName; packaged MapsToCook lists L_MainMenu and L_ArenaM1). Open values: none (label "Main Menu" in English, hardcoded like the menu labels). Scope M2. task-391 and task-392 change the same widget afterwards. No capture subtask (owner Q1, 2026-10-30): the owner checks the result himself in the packaged build. Repo state 2026-10-30: RRPlayerController input fix committed by the owner (091bf23c); Config/DefaultGame.ini (DirectoriesToAlwaysCook) and Config/DefaultEditor.ini have uncommitted owner changes: leave them untouched and never commit them with this task (commit by path).
- confirmed_by_owner: yes (2026-10-30, owner answers relayed by the main session: Q1 no capture, Q2 to Q6 defaults, Q7 changed into task-392)
- milestone: M2
- next_step: task-390-01-return-to-menu-code
- started: open
- waiting_for: none
- result: open

## Questions
Decided 2026-10-30 (owner answers relayed by the main session); values provisional, owner 2026-10-30.
- Q3 L'écran de victoire reste ; son bouton « Play again » devient « Main Menu » et ouvre le menu principal (run remis à zéro, minerai à 0). Answer: oui (défaut).
- Q4 L'écran de défaite garde « Play again » ; le menu s'ouvre directement, sans écran de chargement ni fondu ; même comportement en packagé, -game et PIE. Answer: oui (défaut).

## Subtasks
1. task-390-01-return-to-menu-code (vault/reference/tasks/task-390-run-won-back-to-main-menu/task-390-01-return-to-menu-code.md) — cpp: BlueprintCallable return to the main menu: run reset then L_MainMenu, menu map setting, debug command
2. task-390-02-end-screen-menu-button (vault/reference/tasks/task-390-run-won-back-to-main-menu/task-390-02-end-screen-menu-button.md) — bp: WBP_AlertObjectives: run-won end screen button "Main Menu" calls the return-to-menu entry
3. task-390-03-doc-menu-return (vault/reference/tasks/task-390-run-won-back-to-main-menu/task-390-03-doc-menu-return.md) — doc: update the run flow, main menu, run and end screen docs with the return to the menu

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-390-01-return-to-menu-code]] — step 1, cpp.
- [[task-390-02-end-screen-menu-button]] — step 2, bp.
- [[task-390-03-doc-menu-return]] — step 3, doc.
- [[main-menu]] — the menu spec changed: run won returns to it.
- [[run-structure]] — the run end changed here.
- [[wbp-alert-objectives]] — the end screen widget changed here.
- [[task-391-final-screens-count-level-ore]] — next task, same end screen widget.
