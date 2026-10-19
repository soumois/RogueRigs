---
id: task-230-21-rock-rim-tone-restore
description: "Restore the lighter rim tone in MI_RockSlab (TopParams.b back to 1.3) now that the meshes carry vertex colours."
parent: task-230-rock-slab-integration
kind: task
status: in-progress
---
# Rim tone restored in MI_RockSlab

- goal: the lighter rim band decided by the owner (2026-10-17) shows again: task-230-16 set TopParams.b to 1.0 on a wrong cause (the lighter rectangle came from white vertex colours, task-230-18); with colours imported (task-230-20) the rim multiplier only touches rim triangles. Runs before task-230-19.
- agent: bp
- depends_on: task-230-20-rock-vertex-colour-reimport
- inputs: vault/reference/tech/blueprints/world/bp-rock-slab-material.md, vault/reference/agents/bp-mcp-lessons.md
- expected_output: MI_RockSlab TopParams = (0.5, 0.4, 1.3, 0.55), saved; doc node updated.
- done_when: value read back; asset saved; doc updated; committed.
- editor_state: open
- token_ceiling: 15000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
