---
id: task-452-03-hud-placement
description: "Bp: The health panel sits just above the weapon slots."
parent: task-452-health-above-weapons
kind: task
status: todo
---
# Health bar just above the weapon slots: bp

- goal: The health panel sits just above the weapon slots.
- agent: bp
- depends_on: task-452-02-health-panel
- inputs: wbp-hud (vault/reference/tech/blueprints/ui/wbp-hud.md), wbp-weapons (vault/reference/tech/blueprints/ui/wbp-weapons.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), Art/Source/hud-unified/
- expected_output: see done_when
- done_when: Compiled and saved; anchors and offsets as in A1; PIE shows no runtime error.
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
