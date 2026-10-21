---
id: task-275-04-exposure-freeze
description: "Editor open: PPV_XPGlow AutoExposureMin = Max set to the measured Monday EV100; read-back of lights, robot materials, MPC_PlayerLight and blendable; docs."
parent: task-275-monday-light-revert
kind: task
status: todo
---
# Exposure frozen at the Monday value

- goal: PPV_XPGlow exposure min = max -3.4 -> the EV100 of task-275-03 (overrides on, AEM_Histogram, bias 1, bloom 1.3 / -1, mask blendable weight 1 kept); MPC_PlayerLight read back (Enable 1, FarBrightness 0.03, Radius 250, Exponent 4; set Enable to 1 if it reads 0); read-back of the reverted lights and materials.
- agent: bp
- depends_on: task-275-03-monday-exposure-measure
- inputs: ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md), lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), mpp-player-light-mask (vault/reference/tech/blueprints/world/mpp-player-light-mask.md), bp-robot-readability (vault/reference/tech/blueprints/enemies/bp-robot-readability.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md).
- expected_output: PPV saved by the external actor route; docs ppv-xp-glow and lights-dark-ambient with before -> after and revert (-3.4); read-back table in the report.
- done_when: exposure min and max read back equal to the measured value; sun 6 (1,1,1) shadows true, sky 0.3 (1,1,1) shadows true; chaser masters without Readability parameters; MI_GB_Shooter (0.45, 1, 0.05), MI_GB_Heavy (0.15, 0.35, 0.7); MPC values above; blendable present; git status shows only intended files; lint clean; committed by path; editor left for the Orchestrator to close.
- editor_state: open
- token_ceiling: 30k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-275-monday-light-revert]] — parent global task.
