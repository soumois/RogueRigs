---
id: task-339-06-heal-chunk-doc
description: "Doc: rewrite bp-pickup-heal for the three heal chunk meshes, their materials, collision and Blueprint defaults, from the task-339 change summaries."
parent: task-339-heal-chunk-mesh
kind: task
status: in-progress
---
# Heal pickup chunks: documentation

- goal: update [[bp-pickup-heal]] (description, exposed_defaults, assets, rest_orientation, simple_collision, verified) from the commits and change summaries of task-339-02 to task-339-05: meshes SM_HealChunk_01..03 (tris, size, pivot, slots, source script `Tools/art/blender/heal_chunk.py`), MI_HealChunk_Bright and MI_HealChunk_Dark (parent and values), one simple primitive per variant, the new Blueprint defaults, the capture result. Decided values: global `request` and `gdd_check` of task-339.
- agent: doc
- depends_on: task-339-02-heal-chunk-import, task-339-03-heal-chunk-collision, task-339-04-heal-pickup-blueprint, task-339-05-heal-chunk-capture
- inputs: _authoring (vault/_authoring.md), bp-pickup-heal (vault/reference/tech/blueprints/fx/bp-pickup-heal.md) (to update), bp-pickup-ore (vault/reference/tech/blueprints/fx/bp-pickup-ore.md) (model of the same sections, read only); template `vault/templates/blueprint.md`
- expected_output: bp-pickup-heal updated; one commit.
- done_when: bp-pickup-heal matches the commits (mesh, variants, materials, collision, CastShadow, unchanged defaults); no value invented (unknown written open and reported); lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-28T02:43:28Z
- waiting_for: none
- result: open

## Relations
- [[task-339-heal-chunk-mesh]] — parent global task.
