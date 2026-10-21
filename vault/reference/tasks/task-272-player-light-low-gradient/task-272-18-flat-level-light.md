---
id: task-272-18-flat-level-light
description: "Flat shadowless level lighting (violet sun, low sky, exposure) set to the values chosen from the sweep."
parent: task-272-player-light-low-gradient
kind: task
status: in-progress
---
# Flat level light

- goal: Flat shadowless level lighting (violet sun, low sky, exposure) set to the values chosen from the sweep.
- agent: bp
- depends_on: task-272-16-flat-mask-sweep
- inputs: lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), lights-no-shadows (vault/reference/tech/blueprints/world/lights-no-shadows.md), ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: DirectionalLight_0, SkyLight_0 and exposure set to the flat values chosen; docs with revert.
- done_when: Values read back, level saved, docs updated, lint clean, committed by path.
- editor_state: open
- token_ceiling: 30k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
