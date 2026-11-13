---
id: task-541-02-capture
description: "Capture: Before/after HUD capture with the turret launcher owned at 1920 x 1080"
parent: task-541-hud-turret-count-removed
kind: task
status: todo
---
# HUD: remove the "Turrets: 2 / 2" text above the health bar: capture

- goal: Before/after HUD capture with the turret launcher owned at 1920 x 1080
- agent: capture
- depends_on: task-541-01-remove-text
- inputs: Saved/Scratch/owner-ui-batch-2026-11-13/4.webp
- expected_output: see done_when
- done_when: C1 no "Turrets" text above the health bar; C2 no grey marker left under where it was (per Q4); C3 the launcher slot charge bars visible; C4 health bar and slots unchanged against the before frame.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-541-hud-turret-count-removed]] — parent global task.
