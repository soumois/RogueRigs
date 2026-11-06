---
id: task-480-04-material-loop
description: "Bp: Loop on M_RockSlab/MI_RockSlab: chamfer emissive band with intensity variation, subtle top glow, lit faceted sides with facet tints; capture and compare per Q5."
parent: task-480-rock-slab-chamfer-facets
kind: task
status: wait-answers
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-06T21:13:21Z
- waiting_for: Q5 cap of 4 rounds reached with the emissive band still not showing (A1). Owner: authorise a round 5 for the likely cause (intact pieces read as broken in the material), or say je valide, or give another direction. Sheets in Saved/Captures/Wall480Sheets.
- result: Q5 cap reached, A1 not met, stopped for the owner. Round 1 (Saved/Captures/Wall480R1): large facets visible, sides pink under the miner light, band cream and wide, small orange triangles at the lip. Round 2 (R2): sides violet (SideColor 0.04, 0.016, 0.15, emissive share 0.2), band peach, facet winding fix did not remove the triangles. Round 3 (R3): normals kept on one side along the chain, band colour and intensity changed, no visible change on the edge or the triangles. Round 4 (R4): chamfer position and foot height moved to vertex colour B, no orange band, triangles still there. Failing: A1 (no thin orange-yellow band, only a soft peach glow on the top; the orange is lost, a regression from the before image), lip triangles on the walls. Seen passing on the sheets: A2 and A3 facets and tints, A5 outline and heights, A6 damage, heal and ore crystals; not fully judged: A4 joints in close view, thin pale dotted line along wall feet. Likely cause, inferred and not proven: the material tests broken with UV3.y > 0.0005; intact meshes have only 2 UV channels, so that read may not return 0 on them; with broken true, isRim is 0 (no band, no dark chamfer) and lip faces go through the derivative test, which would explain all the symptoms. Fix needs a round 5, not run because of the cap. A7 stress peace, 100 enemies, one run each at 1920 x 1080, after vs pre-task assets: frame avg 13.09 vs 13.13, game avg 4.84 vs 4.78, GPU avg 11.30 vs 11.37 ms; P99 frame 17.10 vs 15.80 (+8 percent), game 8.92 vs 7.54 (+18 percent, one 30 ms spike in the after run, not repeated), GPU 14.00 vs 13.13 (+7 percent). Averages pass, game P99 over 10 percent on a single run, inconclusive. Sheets Saved/Captures/Wall480Sheets/round4_game.png, round4_side.png, round4_crop_band.png. Commit fedb85d4.

## Relations
- [[task-480-rock-slab-chamfer-facets]] — parent global task.
