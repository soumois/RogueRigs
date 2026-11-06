---
id: task-478-03-doc
description: "Doc: Docs describe the changed wall material."
parent: task-478-wall-lines-emissive-edge
kind: task
status: done
---
# Walls without black lines, emissive on the top edge: doc

- goal: Docs describe the changed wall material.
- agent: doc
- depends_on: task-478-02-capture
- inputs: task-471-wall-look-reference (vault/reference/tasks/task-471-wall-look-reference.md), visual-style (vault/reference/gdd/art-audio/visual-style.md)
- expected_output: see done_when
- done_when: The bp-* node of the wall material describes the removed strata, the emissive placement and the side shading; lint clean; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-478
- usage: included in the executor run of task-478
- runs: 1
- milestone: M2
- started: 2026-11-06T20:17:23Z
- waiting_for: none
- result: f65e4a0e: bp-rock-slab-material gains wall_section (strata removed, sides emissive SideColor with soft vertical streaks and a 0.03 lit share, RockNormal bent-up side normal, RockSpec, top-edge glow from the outline distance of the solid mask, MI_RockSlab values, gaps left open); description updated; tool-unreal-reimport lists rock_slab_wall_section.py. Not edited (gdd, Executor may not): visual-style Walls line and color-code Breakable rock row still say dark stratified sides.

## Relations
- [[task-478-wall-lines-emissive-edge]] — parent global task.
