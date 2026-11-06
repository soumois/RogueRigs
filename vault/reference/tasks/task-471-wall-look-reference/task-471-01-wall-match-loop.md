---
id: task-471-01-wall-match-loop
description: "Bp: Loop: check git history for a matching wall setting, then change the wall look, -game capture, side-by-side with image 2, rework until it matches or the Q5 cap."
parent: task-471-wall-look-reference
kind: task
status: done
---
# Walls look like the owner in-game reference: bp

- goal: Loop: check git history for a matching wall setting, then change the wall look, -game capture, side-by-side with image 2, rework until it matches or the Q5 cap.
- agent: bp
- depends_on: none
- inputs: visual-style (vault/reference/gdd/art-audio/visual-style.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), rock-look-reference (vault/reference/gdd/art-audio/rock-look-reference.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), Art/Source/cartoon-look/reference/owner-2026-11-05-ingame-light-walls.webp, Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-06.webp
- expected_output: see done_when
- done_when: A2 to A6 pass on the last round capture, or the Q5 cap is reached with the remaining gaps listed; A8 finding and every round written in the result; changes committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-471
- usage: included in the executor run of task-471
- runs: 1
- milestone: M2
- started: 2026-11-06T17:31:35Z
- waiting_for: none
- result: A8: git history checked (M_RockSlab/MI_RockSlab log 2a0729f2..58b71b71, grep rim/orange): no earlier wall setting with an orange rim or saturated lilac top; finding none, built new. Change (commit d7d461ea, headless editor Python Tools/unreal/rock_slab_wall_look.py + set_material_instance_params.py): M_RockSlab gains RockRim Custom node (emissive orange on the mesh rim triangles and on a band at the top of side faces) summed with RockMarked, horizontal strata on sides (StrataParams), new params RimColor, RimParams, StrataParams; cause found: ObjectPositionWS reads the instance bounds centre (half block height above floor), so height uses WP.z - OP.z + RimParams.w/2. MI_RockSlab: TopTint (1.15, 0.78, 1.45), SideColor (0.12, 0.04, 0.15), RimColor (1, 0.2, 0.02), RimParams (1.2, 5, 12, 153), StrataParams (44, 0.45, 0.08, 0.35). Rounds (1920x1080 -game, Saved/Captures/Wall471R1..R3, sheets Saved/Captures/Wall471Sheets): R1 tops lilac and sides dark pass, no rim (fail A3); R2 rim params raised, still no rim (fail A3), probes Wall471Dbg..Dbg5 located the bounds-centre cause; R3 (Q5 cap) A2 lilac tops lighter than floor pass, A3 thin orange lip on every wall also far from the miner pass, A4 dark sides with horizontal strata pass, A5 ore and heal crystals keep colours, damaged plates and cracks still show, void asteroids not touched pass, A6 terrain brighter than floor, rim reads as a line not a barrel pass. Remaining gaps vs image 2: target top slightly lighter with a soft warm glow inward from the lip; target sides show vertical column streaks where ours have horizontal strata (A4 wording); seams between sunk broken plates also show thin orange lines.

## Relations
- [[task-471-wall-look-reference]] — parent global task.
