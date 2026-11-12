---
id: task-510-08-main-menu-options-button
description: "Bp: WBP_MainMenu: Options button active and wired to the options screen"
parent: task-510-options-screen
kind: task
status: done
---
# Options screen: Sound, Graphics and Gameplay settings, look from a Kie.ai mockup: bp

- goal: WBP_MainMenu: Options button active and wired to the options screen
- agent: bp
- depends_on: task-510-07-options-widget
- inputs: main-menu (vault/reference/gdd/run/main-menu.md), wbp-main-menu (vault/reference/tech/blueprints/ui/wbp-main-menu.md), ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md)
- expected_output: see done_when
- done_when: WBP_MainMenu compiled; Options no longer greyed, takes the plate hover and opens the options screen; New Run and Exit unchanged.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-510
- usage: included in the executor run of task-510
- runs: 1
- milestone: M3
- started: 2026-11-12T09:52:13Z
- waiting_for: none
- result: 79d97590 (with C++ 2075f53b). WBP_MainMenu: OptionsButton bIsEnabled true in the designer (read back), compiled and saved; no graph change: URRMainMenuWidget binds OptionsButton (click = URROptionsWidget::OpenOptions, enabled when WBP_Options loads) and attaches the plate hover to New Run, Options and Exit. Seen in -game 1920x1080 (Saved/Captures/t500_g3/mm_00.png): Options plate active like New Run and Exit; the screen opens over the menu (t500_g3 sa/gr). New Run and Exit untouched. No focus outline for Options (New Run and Exit have one through RefreshColours): not asked.

## Relations
- [[task-510-options-screen]] — parent global task.
