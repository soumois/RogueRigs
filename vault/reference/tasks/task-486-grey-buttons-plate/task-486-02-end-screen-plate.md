---
id: task-486-02-end-screen-plate
description: "Bp: Run-won Main Menu button of the WBP_AlertObjectives end screen gets the same cream plate as the run-lost Main Menu, with the standard plate hover."
parent: task-486-grey-buttons-plate
kind: task
status: done
---
# Cream plate look for the two grey buttons (level-up Reroll, run-won Main Menu): bp

- goal: Run-won Main Menu button of the WBP_AlertObjectives end screen gets the same cream plate as the run-lost Main Menu, with the standard plate hover.
- agent: bp
- depends_on: task-486-01-reroll-plate
- inputs: ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md), wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), wbp-run-lost (vault/reference/tech/blueprints/ui/wbp-run-lost.md), sys-button-hover (vault/reference/tech/systems/ui/sys-button-hover.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), Saved/Captures/task485_resB/c_won.png
- expected_output: see done_when
- done_when: RestartButton draws as the cream plate at the run-lost Main Menu size with a dark brown label, uses the plate hover instead of the x1.25 grey tint, the rest of the end screen is unchanged and the click still returns to the main menu; compiled with warnings as errors and saved.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-486
- usage: included in the executor run of task-486
- runs: 1
- milestone: M2
- started: 2026-11-07T03:34:06Z
- waiting_for: none
- result: Done (see commit after 3141b2a6). RestartButton wrapped in SizeBox RestartSize 440x108 (EndPanel slot HAlign centre, was fill); WidgetStyle copied from WBP_RunLost MainMenuButton (T_Shop_ButtonContinue Box, margin 0.155); RestartText Roboto Bold 30 dark brown (0.05,0.04,0.03); EndPanel canvas offset top 25 so title, subtitle and Duskite row stay put. Graph untouched. Compiled (warnings as errors) and saved. Seen -game 1080 (Saved/Captures/task486_chk2/sheet.png): cream plate centred under the Duskite row, hover brighter and 1.05 (plate material), rr.Run.MainMenu reaches the main menu, title/subtitle/row at the task-485 positions.

## Relations
- [[task-486-grey-buttons-plate]] — parent global task.
