---
id: task-272-18-flat-level-light
description: "Flat shadowless level lighting (violet sun, low sky, exposure) set to the values chosen from the sweep."
parent: task-272-player-light-low-gradient
kind: task
status: done
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
- model: sonnet
- tokens: input 26, cache write 48078, cache read 469134, output 916
- runs: 1
- milestone: M1
- result: Commit 8b43a5e3. DirectionalLight_0 intensity 0.3 -> 4.0 (violet, no shadows), read back; SkyLight_0 0.08 unchanged; PPV_XPGlow untouched (not read back, checked in task-272-19). Doc lights-dark-ambient: current look paragraph and task-272-18 line, revert sun 0.3.

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
