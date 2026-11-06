---
id: task-480-06-doc
description: "Doc: Docs describe the chamfer, the faceted pieces and the material changes."
parent: task-480-rock-slab-chamfer-facets
kind: task
status: done
---
# Rock slabs: chamfered glowing top edge and faceted low-poly sides: doc

- goal: Docs describe the chamfer, the faceted pieces and the material changes.
- agent: doc
- depends_on: task-480-05-capture
- inputs: bp-rock-slab-material (vault/reference/tech/blueprints/world/bp-rock-slab-material.md), tool-unreal-reimport (vault/reference/tech/pipelines/tool-unreal-reimport.md), rock-state-render-options (vault/reference/tech/systems/world/sys-grid/rock-state-render-options.md), perf-rock-slab (vault/reference/tech/performance/perf-rock-slab.md)
- expected_output: see done_when
- done_when: bp-rock-slab-material describes the chamfer band, top glow, facet tints and lit sides with MI values; the piece generator and variant notes updated where they are documented; triangle counts recorded; lint clean; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-480
- usage: included in the executor run of task-480
- runs: 1
- milestone: M2
- started: 2026-11-06T22:07:33Z
- waiting_for: none
- result: bp-rock-slab-material (chamfer_facets with MI values, uv3_trap), bp-arena-grid (variants), tool-unreal-reimport (two scripts, 100x scale trap), rock-render-piece-set, rock-render-damage, rock-state-render-options, perf-rock-slab (triangle counts, frame times) updated. Lint clean.

## Relations
- [[task-480-rock-slab-chamfer-facets]] — parent global task.
