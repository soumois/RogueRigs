---
id: task-485-01-pause-slice
description: "Bp: Representative slice: hover effect of ui-button-hover on WBP_PauseMenu Resume and Exit, built so the other screens can reuse the same mechanism."
parent: task-485-ui-button-hover
kind: task
status: in-progress
---
# Hover effect on every button of every screen: bp

- goal: Representative slice: hover effect of ui-button-hover on WBP_PauseMenu Resume and Exit, built so the other screens can reuse the same mechanism.
- agent: bp
- depends_on: none
- inputs: ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md), wbp-pause-menu (vault/reference/tech/blueprints/ui/wbp-pause-menu.md), umg-layout (vault/reference/tech/conventions/umg-layout.md)
- expected_output: see done_when
- done_when: In -game with the pause open, hovering Resume or Exit shows the plate brighter and grown to 1.05 with a 0.1 s ease while paused, back to rest on leave; clicks unchanged; no layout change; the mechanism is reusable by the other screens without copying graphs per button; compiled with warnings as errors, saved, no script error.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-07T02:19:47Z
- waiting_for: none
- result: open

## Relations
- [[task-485-ui-button-hover]] — parent global task.
