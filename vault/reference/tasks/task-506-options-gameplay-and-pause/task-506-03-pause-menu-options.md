---
id: task-506-03-pause-menu-options
description: "Bp: Options button in the pause menu."
parent: task-506-options-gameplay-and-pause
kind: task
status: done
---
# Options screen: Gameplay tab and access from the pause menu: bp

- goal: Options button in the pause menu.
- agent: bp
- depends_on: task-506-02-options-gameplay-tab
- inputs: pause-menu (vault/reference/gdd/run/pause-menu.md), wbp-pause-menu (vault/reference/tech/blueprints/ui/wbp-pause-menu.md), art-prompts-pause-menu-mockup (vault/reference/gdd/art-audio/art-prompts-pause-menu-mockup.md), ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md)
- expected_output: see done_when
- done_when: A1 holds; the new button uses the pause menu plate and hover.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-506
- usage: included in the executor run of task-506
- runs: 1
- milestone: M2
- started: 2026-11-12T05:03:15Z
- waiting_for: none
- result: done (cb093af1): OptionsButton + OptionsText 'Options' added to WBP_PauseMenu between Resume and Exit, same T_Shop_ButtonContinue plate and tints, hover and click bound by the C++ base; the three buttons moved to stretch anchors x (0.195-0.385), (0.405-0.595), (0.615-0.805), y (0.819-0.905); labels Resume/Options/Exit 36 -> 30 after the first frame showed them touching the plate caps. Checked in -game (Saved/Captures/t506_03, t506_04_1280x720/pm_00.png): pause menu open, options accepted with menu open 1 paused 1, pause key with options open = Back only (menu open 1, options open 0), log 'closed, focus on Options'; second key resumes. Focus seen through the log only. Mouse hover not driven in -game (same C++ attach as Resume and Exit).

## Relations
- [[task-506-options-gameplay-and-pause]] — parent global task.
