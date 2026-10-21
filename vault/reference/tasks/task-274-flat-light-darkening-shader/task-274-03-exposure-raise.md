---
id: task-274-03-exposure-raise
description: "PPV_XPGlow exposure min = max 1.8 -> -2.9 EV100 so the unmasked scene matches the old masked centre; sun and sky unchanged."
parent: task-274-flat-light-darkening-shader
kind: task
status: todo
---
# Exposure raised for the flat light

- goal: PPV_XPGlow exposure min = max 1.8 -> -2.9 EV100 so the unmasked scene matches the old masked centre; sun and sky unchanged.
- agent: bp
- depends_on: task-274-02-darkener-material
- inputs: ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md), lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: Exposure set and read back; docs with before, after and revert.
- done_when: Exposure min and max read back -2.9; sun 4.0, sky 0.08, bloom 1.3 / -1 read back unchanged; level saved by path; docs updated; lint clean; committed by path.
- editor_state: open
- token_ceiling: 30k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-274-flat-light-darkening-shader]] — parent global task.
