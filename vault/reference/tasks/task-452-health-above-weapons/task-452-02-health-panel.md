---
id: task-452-02-health-panel
description: "Bp: The health panel takes the retained look, effects kept."
parent: task-452-health-above-weapons
kind: task
status: in-progress
---
# Health bar just above the weapon slots: bp

- goal: The health panel takes the retained look, effects kept.
- agent: bp
- depends_on: task-452-01-art
- inputs: wbp-health-status (vault/reference/tech/blueprints/ui/wbp-health-status.md), sys-hud-health-bar (vault/reference/tech/systems/ui/sys-hud-health-bar.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), Art/Source/hud-unified/
- expected_output: see done_when
- done_when: Compiled and saved with no error; effects still driven; PIE hit shows no runtime error.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-05T23:45:51Z
- waiting_for: none
- result: open

## Relations
- [[task-452-health-above-weapons]] — parent global task.
