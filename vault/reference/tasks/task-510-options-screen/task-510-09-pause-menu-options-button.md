---
id: task-510-09-pause-menu-options-button
description: "Bp: WBP_PauseMenu: third button Options between Resume and Exit, wired to the options screen"
parent: task-510-options-screen
kind: task
status: done
---
# Options screen: Sound, Graphics and Gameplay settings, look from a Kie.ai mockup: bp

- goal: WBP_PauseMenu: third button Options between Resume and Exit, wired to the options screen
- agent: bp
- depends_on: task-510-08-main-menu-options-button
- inputs: pause-menu (vault/reference/gdd/run/pause-menu.md), wbp-pause-menu (vault/reference/tech/blueprints/ui/wbp-pause-menu.md), ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md), umg-layout (vault/reference/tech/conventions/umg-layout.md)
- expected_output: see done_when
- done_when: WBP_PauseMenu compiled; an Options plate button in the same style sits between Resume and Exit, takes the plate hover and opens the options screen; stats blocks unchanged.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-510
- usage: included in the executor run of task-510
- runs: 1
- milestone: M3
- started: 2026-11-12T09:52:24Z
- waiting_for: none
- result: def9e5cf (with C++ 2075f53b). WBP_PauseMenu: OptionsButton (style copied from ResumeButton, T_Shop_ButtonContinue) with OptionsText 'Options' (ResumeText font and colour) added to Root; Resume, Options, Exit now 380x100 at anchors x 0.3013, 0.5003, 0.6993, y 0.8619 (were 440x108 at 0.3756 and 0.6274 for two buttons); compiled, saved. URRPauseMenuWidget binds the click (OpenOptions over the paused arena) and the plate hover. Stats blocks unchanged. Seen in -game 1920x1080 (Saved/Captures/t500_pm): three plates with readable labels; options open from the pause menu over the paused arena, game paused.

## Relations
- [[task-510-options-screen]] — parent global task.
