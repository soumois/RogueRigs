---
id: task-506-options-gameplay-and-pause
description: "Global task: Adds the Gameplay tab (damage numbers, aim indicator, screen shake, focus-loss pause) and an Options button to the pause menu."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Options screen: Gameplay tab and access from the pause menu

- request: Owner 2026-11-11, verbatim: "Ok, on va faire l'écran des options [...] Il va falloir passer par l'api chatgpt pour les images ( Kie ). Il faut prévoir aussi les options classiques pour le son, la qualité, et différents élément gamplay." Relay: family gros morceau, with a look to find (preview gate). "api chatgpt ( Kie )" = GPT Image 2 through Kie.ai, Tools/art/kie_image.js. No tests, bot runs or extra captures asked beyond what the rules require for a screen. The request will later be replayed once without the owner from the answers recorded here and in options-menu.
- goal: In a run the player opens Options from the pause menu and turns each Gameplay option of options-menu on or off, with the effect seen in play and kept after a restart.
- acceptance: A1 The pause menu shows Resume, Options, Exit; Options opens the options screen with the game still paused; Back or Escape returns to the pause menu with focus on Options; Escape does not also close the pause menu. A2 A Gameplay tab with each option of options-menu Q5, same look as the other tabs. A3 Each toggle off removes its effect in a run: no damage numbers, no aim chevrons, no screen shake (option left out and recorded if the game has none), no pause on focus loss when off and a pause when on. A4 Values survive a restart; Reset to defaults on the Gameplay tab resets only it. A5 Existing tests a change breaks are updated, none added. A6 Capture at the four resolutions of umg-layout (or as Q9 decided). A7 Doc nodes updated, lint clean, committed.
- gdd_check: Read: run-index, main-menu (Options is a greyed plate that does nothing, "built later"), pause-menu (Resume and Exit only, look from a Kie.ai mockup off the level-end mockup), art-audio-index, ui-button-hover (Options listed as a disabled button), audio-direction (sound palette open, M3), art-prompts, art-prompts-pause-menu-mockup (method and sprites reused), milestones (options screen in no milestone; M3 ended 2026-11-09, so this is post-submission scope, requested by the owner), umg-layout, verification-levels, request-families. No conflict. Open values the work needed: the whole options content, opening points, persistence and look were undecided; proposed in options-menu as Q1 to Q9 defaults. Code facts, 3 targeted reads (step 2 exception) because Q3 depends on them: no sound, audio, music or mix asset under Content/, and no sound call in Source/ (so volume sliders have nothing audible yet); Config/DefaultEngine.ini already sets GameUserSettingsClassName=/Script/RogueRigs.RRGameUserSettings (Source/RogueRigs/Settings/RRGameUserSettings.h exists, content not read). Decisions: gdd node options-menu created (status open until confirmation); confirmation edits main-menu, pause-menu, ui-button-hover, run-index and options-menu.
- verification_level: standard
- family: gros
- confirmed_by_owner: no (plan and questions handed back 2026-11-11)
- milestone: M2
- next_step: task-506-01-settings-gameplay
- started: open
- waiting_for: owner answers to task-504 Q1-Q9
- result: open

## Questions
Asked 2026-11-11, each with a default; open until the owner answers.
- Q1-Q9: asked once on task-504-options-screen-look, they decide this global too.

## Subtasks
1. task-506-01-settings-gameplay (vault/reference/tasks/task-506-options-gameplay-and-pause/task-506-01-settings-gameplay.md) — cpp: Gameplay settings of options-menu stored and obeyed by the game systems.
2. task-506-02-options-gameplay-tab (vault/reference/tasks/task-506-options-gameplay-and-pause/task-506-02-options-gameplay-tab.md) — bp: Gameplay tab in the options screen.
3. task-506-03-pause-menu-options (vault/reference/tasks/task-506-options-gameplay-and-pause/task-506-03-pause-menu-options.md) — bp: Options button in the pause menu.
4. task-506-04-gameplay-capture (vault/reference/tasks/task-506-options-gameplay-and-pause/task-506-04-gameplay-capture.md) — capture: Capture the pause menu access and the Gameplay tab.
5. task-506-05-gameplay-doc (vault/reference/tasks/task-506-options-gameplay-and-pause/task-506-05-gameplay-doc.md) — doc: Update the settings, options widget and pause menu docs.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-506-01-settings-gameplay]] — step 1, cpp.
- [[task-506-02-options-gameplay-tab]] — step 2, bp.
- [[task-506-03-pause-menu-options]] — step 3, bp.
- [[task-506-04-gameplay-capture]] — step 4, capture.
- [[task-506-05-gameplay-doc]] — step 5, doc.
