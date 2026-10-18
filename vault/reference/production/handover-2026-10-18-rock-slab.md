---
id: handover-2026-10-18-rock-slab
description: "State after the rock and ore look session (task-219 done, task-230 at step 06): owner decisions, pending choice, launch rules, how to resume."
parent: production-index
kind: note
---
# Handover — rock slab look session (2026-10-17 to 10-18)

Scope: [[task-219-rock-ore-look]] (done) and [[task-230-rock-slab-integration]] (in progress, next_step `task-230-06-rock-plate-overlay`). Nothing runs; the editor is closed; the Unreal MCP server was not reachable at the start of the session.

## Owner decisions (all recorded in their nodes)
- Look: continuous slab with rounded organic outline (image C, [[art-prompts-rock]]); intact rock smooth, cracks mean damaged ([[color-code]], [[readability-rules]]); cracks as soon as a block is below 100 %.
- Display: dual-grid pieces, display only ([[sys-grid]], [[rock-state-render-options]]); triangle budget provisional, to measure in step 12; rim as vertex colour in one material; Nanite assumed; one variant per shape now, system ready for more.
- A1 level 3: painted, normal and roughness textures allowed ([[art-style-guide]]).
- Heavy damage: no added matter; broken variants of the 5 pieces, plates tilting rigidly toward the empty side, minimum height 60 uu ([[rock-render-damage]], reference captures in [[rock-look-reference]]).
- Weak rock: Perlin layer at generation, capped by a maximum share of the rock (value open); separate task, not created.

## task-230 state
- Done and validated: 01 baseline ([[perf-rock-slab]]), 02 slab pieces, 03 top texture (`Art/Textures/T_RockTop.png`, proposal 2), 04 review (colour too saturated: fix in the 09 material, tint as parameter), 05 crack mask (`Art/Textures/T_RockCrackMask.png`, version 2).
- 06 in progress, waiting for the owner: version 3 committed `5ed04bf2` (10 FBX `SM_RockSlabBroken_<Shape>_{01,02}`, `Tools/art/blender/rock_slab_broken.py`, channel layout and the rigid tilt formula in `Art/Meshes/rock-slab/SM_RockSlabBroken.report.json`). Open question to the owner: fix the thin spiky plates near the void (minimum plate size) before 07, or accept as is. Old shard cluster files `SM_RockPlates_01*` are superseded (keep or delete: owner).
- For 09, already decided: per-plate random seed taken from a world-position hash, not only the stored seed (removes the repeated tilt pattern); desaturated top, tint parameter; full-precision UVs and 4 UV channels at import (step 11).
- Unspecified: damage value of an empty corner cell for the sag (previews used the max of solid neighbours); 07/08 decide and document.

## Launch rules (owner)
03 to 06 one by one with his validation; 07 to 12 run as one sequence without asking. 07 and 08 need the editor closed; 09 to 11 need it open with the MCP server up (launch it, poll `http://127.0.0.1:8000/mcp`); 12 needs it closed.

## Resume
Read [[task-230-rock-slab-integration]], then the 06 node; send the owner the 06 previews in `Art/Meshes/rock-slab/preview/rockbroken_*` if he has not seen them, and ask the open question above.

## Relations
- [[production-index]] — parent index.
- [[task-230-rock-slab-integration]] — the task in progress.
- [[task-219-rock-ore-look]] — the look exploration behind it.
- [[rock-state-render-options]] — the design.
