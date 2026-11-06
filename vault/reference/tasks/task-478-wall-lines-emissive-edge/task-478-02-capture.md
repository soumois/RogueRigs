---
id: task-478-02-capture
description: "Capture: Final independent check of the walls against image 3 and the before."
parent: task-478-wall-lines-emissive-edge
kind: task
status: done
---
# Walls without black lines, emissive on the top edge: capture

- goal: Final independent check of the walls against image 3 and the before.
- agent: capture
- depends_on: task-478-01-wall-match-loop
- inputs: agent-capture (vault/reference/agents/agent-capture.md), Art/Source/cartoon-look/reference/owner-2026-11-06-wall-black-lines.png, Art/Source/cartoon-look/reference/owner-2026-11-06-wall-section-target.png
- expected_output: see done_when
- done_when: C1 no dark line on wall sides; C2 no visible cut between facets or blocks; C3 orange glow on the top face edge, fading inward, thin; C4 top surface without seams; C5 damage cracks and plates still show; C6 walls separate from the floor; frames at the game camera and a low side view; sheets sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-478
- usage: included in the executor run of task-478
- runs: 1
- milestone: M2
- started: 2026-11-06T20:15:51Z
- waiting_for: none
- result: Capture Saved/Captures/Wall478Final (1 launch, 1920x1080 -game: game, low, damaged, far, void), sheets Saved/Captures/Wall478Sheets/final_game_camera.png and final_low_view.png (before, after, image 3; low view also with image 2). C1 no dark line on the sides: seen (low_00, game_00). C2 no cut between facets or blocks: ambiguous, owner: facet and shadow cuts gone and walls read as one surface at the game camera, but close views show small dotted light seams on some block joints of the sides (damaged_00). C3 orange glow on the top face edge fading inward, thin: seen on every wall, also far from the miner (far_00); thinner and weaker than image 3. C4 top surface without seams: seen (far_00, game_00). C5 damage plates and cracks: seen on the damaged block (damaged_00, sunk plates with orange plate seams). C6 walls separate from the floor: seen. Also: sides a brighter, more saturated purple than the dark purple of image 3 (A3 colour gap); barrels and void unchanged (void_00); heal ore not in frame. run.log error lines identical to the before run.

## Relations
- [[task-478-wall-lines-emissive-edge]] — parent global task.
