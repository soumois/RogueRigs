---
id: task-480-04-material-loop
description: "Bp: Loop on M_RockSlab/MI_RockSlab: chamfer emissive band with intensity variation, subtle top glow, lit faceted sides with facet tints; capture and compare per Q5."
parent: task-480-rock-slab-chamfer-facets
kind: task
status: done
---
# Rock slabs: chamfered glowing top edge and faceted low-poly sides: bp

- goal: Loop on M_RockSlab/MI_RockSlab: chamfer emissive band with intensity variation, subtle top glow, lit faceted sides with facet tints; capture and compare per Q5.
- agent: bp
- depends_on: task-480-03-reimport-pieces
- inputs: bp-rock-slab-material (vault/reference/tech/blueprints/world/bp-rock-slab-material.md), visual-style (vault/reference/gdd/art-audio/visual-style.md), color-code (vault/reference/gdd/art-audio/color-code.md), Art/Source/cartoon-look/reference/owner-2026-11-06-wall-after-478.png
- expected_output: see done_when
- done_when: A1 to A6 and A9 pass on the loop sheets, or the Q5 round cap is reached, or the owner said "je valide"; each round written in the result; frame times of A7 measured; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-480
- usage: included in the executor run of task-480
- runs: 1
- milestone: M2
- started: 2026-11-06T21:56:35Z
- waiting_for: none
- result: Owner on the cap, verbatim: "rajoute 3 possible" (rounds 5 to 7 allowed, 6 used). Round 1 (Saved/Captures/Wall480R1): facets visible, sides pink under the miner light, band cream and wide, orange triangles at the lip. Round 2 (R2): sides violet, band peach; winding fix did not remove the triangles. Round 3 (R3): normals kept on one side along the chain, new band values, no visible change. Round 4 (R4): chamfer position and foot height moved to vertex colour B, still no band. Diagnostic between rounds 4 and 5 (Saved/Captures/Wall480Diag): emissive showing the flag drew every intact piece as broken, so the reported cause held: UV3.y > 0.0005 is true on intact meshes (2 UV channels). Round 5 (R5): material no longer reads UV3 (sides from vertex colour G, white meshes from derivatives), broken tops recoded (0,0,0), 10 broken meshes reimported: thin orange-yellow band on, triangles gone, pale line at wall feet gone. Round 6 (R6, final): softer band (RimParams.x 1.5, BandParams 8, 0.45, 0.4, 0.5), chamfer albedo TopParams.b 0.55, top glow EdgeParams 8, 16, 0.14, 67, SideColor 0.05, 0.016, 0.13. A1 to A6 and A9 pass on the sheets (A9 as -game runs, no new error; PIE not run); to note for the owner: sides close to the miner light lean magenta. Round 7 not needed. A7 stress peace, 100 enemies, 2 runs on the final material vs the 1 pre-task run: frame avg 12.25 and 12.63 vs 13.13, P99 14.70 and 15.51 vs 15.80; game avg 4.32 and 4.46 vs 4.78, P99 6.40 and 6.67 vs 7.54; GPU avg 10.71 and 10.96 vs 11.37, P99 12.49 and 12.90 vs 13.13 ms: not worse. The +18 percent game P99 of the round 4 run did not repeat. Commits fedb85d4, 96cf70f0.

## Relations
- [[task-480-rock-slab-chamfer-facets]] — parent global task.
