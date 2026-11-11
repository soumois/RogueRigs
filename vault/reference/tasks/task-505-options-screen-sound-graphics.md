---
id: task-505-options-screen-sound-graphics
description: "Global task: Options screen from the task-504 look: Sound and Graphics tabs, settings saved and applied at launch, opened from the main menu."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Options screen with Sound and Graphics, opened from the main menu

- request: Owner 2026-11-11, verbatim: "Ok, on va faire l'écran des options [...] Il va falloir passer par l'api chatgpt pour les images ( Kie ). Il faut prévoir aussi les options classiques pour le son, la qualité, et différents élément gamplay." Relay: family gros morceau, with a look to find (preview gate). "api chatgpt ( Kie )" = GPT Image 2 through Kie.ai, Tools/art/kie_image.js. No tests, bot runs or extra captures asked beyond what the rules require for a screen. The request will later be replayed once without the owner from the answers recorded here and in options-menu.
- goal: From the main menu the player opens Options, changes every Sound and Graphics setting of options-menu, and finds them again after a restart.
- acceptance: A1 The main menu Options plate is active, with the plate hover, and opens the options screen; Back or Escape returns to the menu with focus on Options. A2 The screen matches the validated task-504 mockup and follows umg-layout: title, Sound and Graphics tabs (Gameplay comes in task-506), Reset to defaults, Back, button hover of ui-button-hover. A3 Every Sound and Graphics option of options-menu is present with its current value and applies at once: window mode, resolution, VSync, frame rate limit, preset and its five details (preset reads Custom when one detail differs), resolution scale; the three volumes reach their volume groups (read back, no sound exists). A4 Values survive a game restart (Q6); Reset to defaults resets the current tab only. A5 First-launch graphics values recorded in options-menu. A6 Existing tests a change breaks are updated, none added. A7 Capture at the four resolutions of umg-layout (or as Q9 decided), compared with the validated mockup. A8 Doc nodes created or updated, lint clean, committed.
- gdd_check: Read: run-index, main-menu (Options is a greyed plate that does nothing, "built later"), pause-menu (Resume and Exit only, look from a Kie.ai mockup off the level-end mockup), art-audio-index, ui-button-hover (Options listed as a disabled button), audio-direction (sound palette open, M3), art-prompts, art-prompts-pause-menu-mockup (method and sprites reused), milestones (options screen in no milestone; M3 ended 2026-11-09, so this is post-submission scope, requested by the owner), umg-layout, verification-levels, request-families. No conflict. Open values the work needed: the whole options content, opening points, persistence and look were undecided; proposed in options-menu as Q1 to Q9 defaults. Code facts, 3 targeted reads (step 2 exception) because Q3 depends on them: no sound, audio, music or mix asset under Content/, and no sound call in Source/ (so volume sliders have nothing audible yet); Config/DefaultEngine.ini already sets GameUserSettingsClassName=/Script/RogueRigs.RRGameUserSettings (Source/RogueRigs/Settings/RRGameUserSettings.h exists, content not read). Decisions: gdd node options-menu created (status open until confirmation); confirmation edits main-menu, pause-menu, ui-button-hover, run-index and options-menu.
- verification_level: standard
- family: gros
- confirmed_by_owner: no (plan and questions handed back 2026-11-11)
- milestone: M2
- next_step: task-505-01-settings-sound-graphics
- started: open
- waiting_for: owner answers to task-504 Q1-Q9
- result: open

## Questions
Asked 2026-11-11, each with a default; open until the owner answers.
- Q1-Q9: asked once on task-504-options-screen-look, they decide this global too.

## Subtasks
1. task-505-01-settings-sound-graphics (vault/reference/tasks/task-505-options-screen-sound-graphics/task-505-01-settings-sound-graphics.md) — cpp: Sound and Graphics settings of options-menu stored, applied at once and at launch, exposed to the widget.
2. task-505-02-options-widget (vault/reference/tasks/task-505-options-screen-sound-graphics/task-505-02-options-widget.md) — bp: Options screen widget with Sound and Graphics tabs, built from the task-504 elements.
3. task-505-03-main-menu-options (vault/reference/tasks/task-505-options-screen-sound-graphics/task-505-03-main-menu-options.md) — bp: Main menu Options button opens the options screen.
4. task-505-04-options-capture (vault/reference/tasks/task-505-options-screen-sound-graphics/task-505-04-options-capture.md) — capture: Capture the options screen from the main menu.
5. task-505-05-options-doc (vault/reference/tasks/task-505-options-screen-sound-graphics/task-505-05-options-doc.md) — doc: Document the settings system and the options widget.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-505-01-settings-sound-graphics]] — step 1, cpp.
- [[task-505-02-options-widget]] — step 2, bp.
- [[task-505-03-main-menu-options]] — step 3, bp.
- [[task-505-04-options-capture]] — step 4, capture.
- [[task-505-05-options-doc]] — step 5, doc.
