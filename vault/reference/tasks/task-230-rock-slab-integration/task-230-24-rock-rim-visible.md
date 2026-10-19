---
id: task-230-24-rock-rim-visible
description: "The lighter rim band along the rock outline is visible at the game camera."
parent: task-230-rock-slab-integration
kind: task
status: done
---
# The lighter rim band along the rock outline is visible at the game camera

- goal: the rim band decided by the owner (2026-10-17) reads at the game camera (owner 2026-10-19, after the 230-19 captures (Saved/RockSlab/after3/): not visible on the pale tops). After task-230-23, check the rim triangles get TopParams.b and raise the contrast of the band (rim tone, or a slight darkening of the top next to it) until it reads; report the values.
- agent: bp
- depends_on: task-230-23-rock-top-lavender
- inputs: vault/reference/tech/blueprints/world/bp-rock-slab-material.md, vault/reference/agents/bp-mcp-lessons.md, Art/Meshes/rock-slab/SM_RockSlab.report.json
- expected_output: MI_RockSlab rim values (and material change if needed), doc updated.
- done_when: saved; doc updated; committed; look checked in task-230-26.
- editor_state: open
- token_ceiling: 30000
- model: sonnet
- tokens: input 24, cache write 27093, cache read 240488, output 394
- runs: 1
- milestone: M2
- result: Commit 7f3723d3. MI_RockSlab TopParams.b 1.3 -> 2.0 (rim about 2x the top mean, estimate); no HLSL change. Look checked in 230-26.

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
