---
id: task-485-03-run-lost
description: "Bp: Same hover on WBP_RunLost Retry and Main Menu."
parent: task-485-ui-button-hover
kind: task
status: done
---
# Hover effect on every button of every screen: bp

- goal: Same hover on WBP_RunLost Retry and Main Menu.
- agent: bp
- depends_on: task-485-02-slice-capture
- inputs: ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md), wbp-run-lost (vault/reference/tech/blueprints/ui/wbp-run-lost.md)
- expected_output: see done_when
- done_when: Retry and Main Menu show the hover of ui-button-hover in -game, appearance animation and skip unchanged, clicks unchanged; compiled with warnings as errors, saved.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-485
- usage: included in the executor run of task-485
- runs: 1
- milestone: M2
- started: 2026-11-07T02:30:38Z
- waiting_for: none
- result: Done (ca37b1da). URRRunLostWidget::NativeOnInitialized attaches RetryButton and MainMenuButton to URRButtonHoverSubsystem; SkipButton not attached; appearance (opacity only) untouched. Build succeeded; no asset change. Seen in -game: plates brighter and grown when hovered at 1920x1080, 1280x720, 1920x1200, 2560x1080 (Saved/Captures/task485_a1080/rl_crop.png, task485_resA/sheet_rl.png, task485_rl1200/contact_sheet.png); appearance plays and ends with focus on Retry. Retry click not exercised (OnClicked binding unchanged).

## Relations
- [[task-485-ui-button-hover]] — parent global task.
