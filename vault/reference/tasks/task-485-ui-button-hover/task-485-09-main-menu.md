---
id: task-485-09-main-menu
description: "Bp: Main menu New Run and Exit: keep the orange and add the growth of ui-button-hover."
parent: task-485-ui-button-hover
kind: task
status: done
---
# Hover effect on every button of every screen: bp

- goal: Main menu New Run and Exit: keep the orange and add the growth of ui-button-hover.
- agent: bp
- depends_on: task-485-08-end-screen
- inputs: ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md), wbp-main-menu (vault/reference/tech/blueprints/ui/wbp-main-menu.md), main-menu (vault/reference/gdd/run/main-menu.md)
- expected_output: see done_when
- done_when: New Run and Exit turn orange and grow to 1.05 with a 0.1 s ease on hover, back on leave, Options shows nothing, focus colour unchanged; compiled with warnings as errors, saved.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-485
- usage: included in the executor run of task-485
- runs: 1
- milestone: M2
- started: 2026-11-07T03:03:15Z
- waiting_for: none
- result: Done (76cd99d1). URRMainMenuWidget::NativeOnInitialized attaches NewRunButton and ExitButton with growth only (no plate); the orange stays the Blueprint focus colour (hover sets keyboard focus). Options not attached and disabled. Seen in -game: hovered New Run / Exit orange and grown, Options hovered no change; focus-only orange shows no growth (Saved/Captures/task485_b1080/c_mm.png, task485_resB/sheet_mm.png, task485_resB1200).

## Relations
- [[task-485-ui-button-hover]] — parent global task.
