---
id: task-485-08-end-screen
description: "Bp: Same hover on the WBP_AlertObjectives end screen button (run-won Main Menu)."
parent: task-485-ui-button-hover
kind: task
status: done
---
# Hover effect on every button of every screen: bp

- goal: Same hover on the WBP_AlertObjectives end screen button (run-won Main Menu).
- agent: bp
- depends_on: task-485-07-upgrade-reroll
- inputs: ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md), wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md)
- expected_output: see done_when
- done_when: The end screen button shows the hover of ui-button-hover, click unchanged; compiled with warnings as errors, saved.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-485
- usage: included in the executor run of task-485
- runs: 1
- milestone: M2
- started: open
- waiting_for: none
- result: Done (0b3bee79). WBP_AlertObjectives default HoverButtonNames = RestartButton, compiled with warnings as errors and saved (WBP_HUD got dirty from the child recompile; Don't Save, unchanged). Untextured grey button with white label: same tint path as 07; seen at the four resolutions (Saved/Captures/task485_resB/c_won.png, sheet_won.png, task485_resB1200): lighter and grown, label readable. Click not exercised (rr.Run.MainMenu calls the same run flow).

## Relations
- [[task-485-ui-button-hover]] — parent global task.
