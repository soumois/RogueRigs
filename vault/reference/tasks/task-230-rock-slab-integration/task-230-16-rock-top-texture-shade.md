---
id: task-230-16-rock-top-texture-shade
description: "Rock top shows its painted texture and one even tint: no rectangle where full pieces meet edge pieces."
parent: task-230-rock-slab-integration
kind: task
status: in-progress
---
# Rock top shows its painted texture and one even tint: no rectangle where full pieces meet edge pieces

- goal: the slab top shows the T_RockTop texture (230-12 captures show a flat plain colour) and full pieces have the same shade as edge, corner and diagonal pieces (owner 2026-10-18, after the 230-12 check: a lighter rectangle appears inside each mass, see Saved/RockSlab/after/rs_f_digearly_00.png). Check vertex colours of SM_RockSlab_Full_01 (2 tris) against the other pieces, the top/rim/side thresholds and the world-UV sampling in M_RockSlab.
- agent: bp
- depends_on: task-230-15-duskite-crystal-colour
- inputs: vault/reference/tech/blueprints/world/bp-rock-slab-material.md, vault/reference/tech/systems/world/sys-grid/rock-state-render-options/rock-render-top-texture.md, vault/reference/tech/systems/world/sys-grid/rock-state-render-options/rock-render-piece-set.md, vault/reference/agents/bp-mcp-lessons.md; Art/Meshes/rock-slab/SM_RockSlab.report.json; captures Saved/RockSlab/after/
- expected_output: cause, fix in M_RockSlab or MI_RockSlab (or the mesh change needed, reported), doc node updated.
- done_when: material compiles; doc updated; committed; the look is checked in task-230-17.
- editor_state: open
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
