---
id: task-478-01-wall-match-loop
description: "Bp: Loop: remove the dark lines and visible cuts, place the emissive on the top edge, capture, compare with image 3, rework; at most 4 rounds."
parent: task-478-wall-lines-emissive-edge
kind: task
status: done
---
# Walls without black lines, emissive on the top edge: bp

- goal: Loop: remove the dark lines and visible cuts, place the emissive on the top edge, capture, compare with image 3, rework; at most 4 rounds.
- agent: bp
- depends_on: none
- inputs: task-471-wall-look-reference (vault/reference/tasks/task-471-wall-look-reference.md), visual-style (vault/reference/gdd/art-audio/visual-style.md), rock-look-reference (vault/reference/gdd/art-audio/rock-look-reference.md), Art/Source/cartoon-look/reference/owner-2026-11-06-wall-black-lines.png, Art/Source/cartoon-look/reference/owner-2026-11-06-wall-section-target.png
- expected_output: see done_when
- done_when: A2 to A7 and A9 pass on the loop sheets, or 4 rounds are done, or the owner said "je valide"; each round written in the result; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-478
- usage: included in the executor run of task-478
- runs: 1
- milestone: M2
- started: 2026-11-06T20:00:00Z
- waiting_for: none
- result: Cap of 4 rounds reached (A8); commit 4279e647, headless editor Python Tools/unreal/rock_slab_wall_section.py. Change: M_RockSlab loses the task-471 strata (StrataParams deleted); sides are drawn as emissive SideColor with soft vertical streaks and the foot darkening, a lit share SideParams.x 0.03 only, a world normal bent up (RockNormal, tangent_space_normal off) and specular 0 (RockSpec), so light, shadow and facets no longer cut the wall; RockRim adds an orange glow on the top face fading inward over 16 uu from the piece outline, distance computed from the per-instance solid mask (line or quarter circles of rock_slab_pieces.py, continuous across pieces), the band under the lip reduced (RimParams 1.6, 2, 5, 153). MI_RockSlab: SideColor (0.06, 0.018, 0.085), SideParams (0.03, 1, 25, 0.6), EdgeParams (-1, 16, 3, 67). Rounds (1920x1080 -game, game camera + low view, Saved/Captures/Wall478Before, Wall478R1..R4, sheets Saved/Captures/Wall478Sheets): R1 strata gone, sides lit too bright, top glow a wide grid-aligned staircase (bilinear mask field) fail A3 A4; R2 glow from the outline distance hugs the edge pass A4, sides still bright and shadow-cut fail A3; R3 side specular removed, no change fail A3; probe Wall478Dbg1 (green SideColor) showed the side is lit strongly with its bent normal and cut by shadows; R4 sides emissive: no strata, no facet or block cut, soft vertical streaks pass A2 A3 except colour, top glow thin and visible far from the miner pass A4, tops seamless pass A5, crystals and damaged block cracks show pass A6, walls separate from the floor pass A7, run.log error lines identical to the before run pass A9. Gaps left for the owner after round 4: sides a brighter, more saturated purple than the dark purple of image 3; top glow thinner and weaker than image 3; streaks softer than the target; a few small dotted light seams on some block joints of the sides; heal ore and void asteroids not in these frames.

## Relations
- [[task-478-wall-lines-emissive-edge]] — parent global task.
