---
id: task-452-04-doc
description: "Doc: Docs describe the new place and look."
parent: task-452-health-above-weapons
kind: task
status: done
---
# Health bar just above the weapon slots: doc

- goal: Docs describe the new place and look.
- agent: doc
- depends_on: task-452-03-hud-placement
- inputs: wbp-health-status (vault/reference/tech/blueprints/ui/wbp-health-status.md), wbp-hud (vault/reference/tech/blueprints/ui/wbp-hud.md), sys-hud-health-bar (vault/reference/tech/systems/ui/sys-hud-health-bar.md), blueprints-ui-index (vault/reference/tech/blueprints/ui/blueprints-ui-index.md)
- expected_output: see done_when
- done_when: Anchors listed per umg-layout; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-452
- usage: included in the executor run of task-452
- runs: 1
- milestone: M2
- started: 2026-11-05T23:52:00Z
- waiting_for: none
- result: Commit 1d8ad8f6. Updated wbp-health-status (layout bullet: HealthPanel anchors 0.5,1, alignment 0.5,1, offsets 0/-148, 652 x 44, children and slots, plate brush, end-screen target, verified), wbp-hud (Subtitles -240, task-452 layout check), wbp-weapons (TurretBox bottom padding 52), sys-hud-health-bar (placement, punch overlap), blueprints-ui-index gloss. Lint clean (warnings only, pre-existing).

## Relations
- [[task-452-health-above-weapons]] — parent global task.
