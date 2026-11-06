---
id: task-471-03-doc
description: "Doc: Docs describe the new wall materials."
parent: task-471-wall-look-reference
kind: task
status: done
---
# Walls look like the owner in-game reference: doc

- goal: Docs describe the new wall materials.
- agent: doc
- depends_on: task-471-02-capture
- inputs: visual-style (vault/reference/gdd/art-audio/visual-style.md), color-code (vault/reference/gdd/art-audio/color-code.md)
- expected_output: see done_when
- done_when: The doc nodes of the wall materials touched describe the new look and values; lint clean; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-471
- usage: included in the executor run of task-471
- runs: 1
- milestone: M2
- started: 2026-11-06T17:52:07Z
- waiting_for: none
- result: Commit 1f879054: bp-rock-slab-material gains wall_look (RockRim emissive formula, Add into Emissive, height from the instance bounds centre, side strata, MI_RockSlab and parent default values, in-game observation); description updated; tool-unreal-reimport lists rock_slab_wall_look.py and the no-space path pitfall; relations on both ends. GDD nodes visual-style and color-code NOT edited: the Executor must not change gdd nodes; the Q3 update (rock light lilac-violet, thin orange lip allowed beside 'orange means explodes') is left for the owner or planner. Lint clean.

## Relations
- [[task-471-wall-look-reference]] — parent global task.
