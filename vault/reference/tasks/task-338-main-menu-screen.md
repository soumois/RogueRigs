---
id: task-338-main-menu-screen
description: "Global task: Main menu screen at game start: background image, New Run starts a run, Options placeholder, Exit quits the game."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Main menu screen

- request: "Ok maintenant, on va travailler au design du main menu. Travaille avec ce que tu as et les images que tu as et chatpgpt pour me faire un bon background, un menu ( new run , option qu'on fera plus tard et exit )." (relayed verbatim by the main session, owner 2026-10-28). This task covers the point "un menu ( new run , option qu'on fera plus tard et exit )"; the background image is [[task-337-main-menu-background]].
- goal: Main menu screen at game start: background image, New Run starts a run, Options placeholder, Exit quits the game.
- acceptance: a -game launch opens the main menu over the picked background (capture at 1920 x 1080 sent to the owner); New Run reaches level 1 of a fresh run on L_ArenaM1; Options is shown and does nothing; Exit closes the game; PIE on L_ArenaM1 still starts a run directly; doc nodes written; committed.
- gdd_check: no main menu exists in the GDD, tech or wishlist (descent gdd-index > run-index; tech > blueprints-ui-index, systems-ui-index). Today any map starts level 1 directly ([[sys-run]]: "PIE or -game on any map starts level 1, no menu"); GameDefaultMap and EditorStartupMap are both L_ArenaM1 (Config/DefaultEngine.ini). Read [[sys-run]] (ResetRun, BeginRun), [[sys-run-transition]] (loading and travel), [[wbp-loading]] and [[wbp-intermission]] (screen look: Roboto Bold, white with outline, orange 1/0.35/0.1 accent, texts from screen_texts.json), [[text-content-files]] (screen_texts.json items), [[cpp-bp-split]], [[milestones]] (a main menu is in no milestone). Open values: Q1 and Q6 to Q11 of the plan, asked 2026-10-28 with defaults (full list in the gdd_check of [[task-337-main-menu-background]]); answers pending. Decisions go to a new gdd node main-menu (vault/reference/gdd/run/main-menu.md) once answered.
- confirmed_by_owner: no (questions asked 2026-10-28)
- milestone: M3
- next_step: task-338-01-main-menu-screen
- started: open
- waiting_for: owner answers to Q1 to Q11 and confirmation of the plan
- result: open

## Subtasks
1. task-338-01-main-menu-screen (vault/reference/tasks/task-338-main-menu-screen/task-338-01-main-menu-screen.md) — cpp: C++ main menu game mode, widget base (NewRun, Exit, Options disabled) and rr.Menu debug commands.
2. task-338-02-wbp-main-menu (vault/reference/tasks/task-338-main-menu-screen/task-338-02-wbp-main-menu.md) — bp: WBP_MainMenu widget with the imported background texture, title and three buttons.
3. task-338-03-bp-main-menu-game-mode (vault/reference/tasks/task-338-main-menu-screen/task-338-03-bp-main-menu-game-mode.md) — bp: BP_MainMenuGameMode, child of the C++ game mode, with WBP_MainMenu assigned.
4. task-338-04-main-menu-map (vault/reference/tasks/task-338-main-menu-screen/task-338-04-main-menu-map.md) — bp: L_MainMenu map with the game mode override, set as GameDefaultMap.
5. task-338-05-main-menu-capture (vault/reference/tasks/task-338-main-menu-screen/task-338-05-main-menu-capture.md) — cpp: -game capture of the menu and log check of New Run and Exit.
6. task-338-06-main-menu-doc (vault/reference/tasks/task-338-main-menu-screen/task-338-06-main-menu-doc.md) — doc: system and Blueprint doc nodes of the main menu.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-338-01-main-menu-screen]] — step 1, cpp.
- [[task-338-02-wbp-main-menu]] — step 2, bp.
- [[task-338-03-bp-main-menu-game-mode]] — step 3, bp.
- [[task-338-04-main-menu-map]] — step 4, bp.
- [[task-338-05-main-menu-capture]] — step 5, cpp.
- [[task-338-06-main-menu-doc]] — step 6, doc.
- [[task-337-main-menu-background]] — the background image this screen shows; its pick must exist before step 2.
