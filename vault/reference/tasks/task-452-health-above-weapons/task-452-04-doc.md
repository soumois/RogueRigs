---
id: task-452-04-doc
description: "Doc: Docs describe the new place and look."
parent: task-452-health-above-weapons
kind: task
status: todo
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-452-health-above-weapons]] — parent global task.
