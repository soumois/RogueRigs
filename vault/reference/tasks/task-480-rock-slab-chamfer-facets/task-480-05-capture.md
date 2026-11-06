---
id: task-480-05-capture
description: "Capture: Final independent check of the slabs against the before image."
parent: task-480-rock-slab-chamfer-facets
kind: task
status: done
---
# Rock slabs: chamfered glowing top edge and faceted low-poly sides: capture

- goal: Final independent check of the slabs against the before image.
- agent: capture
- depends_on: task-480-04-material-loop
- inputs: agent-capture (vault/reference/agents/agent-capture.md), Art/Source/cartoon-look/reference/owner-2026-11-06-wall-after-478.png
- expected_output: see done_when
- done_when: C1 thin orange-yellow band on a chamfer of the top edge, slight intensity variation, not neon; C2 subtle glow on the top near the edge; C3 large vertical facets visible on the sides, irregular edges, no cracks, no noisy detail, no obvious repetition; C4 dark, mid and lighter violet facets, lit and shaded alternating; C5 top texture unchanged and continuous, no line between slabs, no seam at side joints; C6 outline and heights as before; C7 damaged rock still reads damaged; heal deposits and ore crystals keep their colours; frames at the game camera and a low side view at 1920 x 1080; sheets sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-480
- usage: included in the executor run of task-480
- runs: 1
- milestone: M2
- started: 2026-11-06T22:05:37Z
- waiting_for: none
- result: C1 seen (Saved/Captures/Wall480Sheets/final_side.png, joints and low_side; Wall480Final/f4_far_00.png): thin orange-yellow band on the chamfer, thickness and intensity vary, not neon-like at game distance. C2 seen (f4_far, f5_heal): subtle glow on the top near the edge. C3 seen for large facets, irregular edges, no cracks, no noise (f3_joints); AMBIGUOUS on repetition: on walls far from the miner the facets read as rather regular dark and light stripes (final_crop_damage.png), owner to judge. C4 seen for dark, mid and lighter facets, lit and shaded alternating (f2_low, f3_joints); AMBIGUOUS on hue: sides lean magenta close to the miner light and indigo far from it, owner to judge. C5 seen: top texture unchanged and continuous, no line between slabs (f4_far); no gap at side joints in f3_joints, one thin dark line between two facets low on that wall; the pale dotted lines of the task-478 sides and the pale line at wall feet are gone. C6 seen on the frames against the before image: outline kept by construction (outline_pts unchanged), heights as before, level with the miner top not measured again. C7 seen: damaged rock reads damaged (cracks, sunk plates, f5_heal, f3_joints), heal deposit and ore crystals keep their colours; AMBIGUOUS detail: sunk plate tops show saturated orange patches (final_crop_damage.png), and the band stops along a damaged stretch per Q3. 1 launch, final frames Saved/Captures/Wall480Final, no new error in run.log. Sheets for the owner: Saved/Captures/Wall480Sheets/final_game.png, final_side.png, final_crop_damage.png. Captures only, no generated image, so no style-guide run.

## Relations
- [[task-480-rock-slab-chamfer-facets]] — parent global task.
