---
id: task-151-13-chaser-torso-pass
description: "Second pass on the chaser torso: a modeller who does not judge, and an independent critic who lists the visible differences."
parent: task-151-chaser-robot
kind: task
status: done
---
# Chaser torso: pass with an independent critic

- goal: Second pass on the chaser torso: a modeller who does not judge, and an independent critic who lists the visible differences.
- agent: 3d
- depends_on: task-151-12-chaser-assembly
- inputs: chaser-model-pipeline (vault/reference/tech/pipelines/chaser-model-pipeline.md), mesh-fidelity-loop (vault/reference/tech/pipelines/mesh-fidelity-loop.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), agent-3d (vault/reference/agents/agent-3d.md), agent-style-guide (vault/reference/agents/agent-style-guide.md)
- expected_output: Tools/art/blender/chaser/part_headtorso.py (and a --lit option in part_preview.py), renders of the torso flat and lit, the critic reviews of each round, the rebuilt assembly if the torso changed
- done_when: the independent critic (a different agent on a different model, read only) finds no remaining difference of high or medium severity between the torso renders and the sheet crops, or 3 rounds are used and the remaining differences are reported; the modeller never grades its own work; interface of the part unchanged (name, pivot, parent, attachment section, bounds within 2 cm); at most 600 triangles for the part, 3000 in total; committed by name
- process (owner 2026-10-13: the agent must not judge itself): the Orchestrator runs rounds. Round N: (1) the modeller (3d agent) changes the torso using the critic list of the previous round (the first round uses the known differences below) and produces renders, WITHOUT any pass or fail statement, only a list of what it changed; (2) a fresh critic agent (general-purpose, model opus, read only: it never edits a file) opens the renders and the sheet crops, compares them at three scales (part scale, full robot, and the in-game scale: the robot downscaled to about 70 pixels tall seen at the camera pitch of RRMiner) and returns a numbered list of visible differences with a severity (high, medium, low) and an overall verdict.
- known differences (first round): the back of the sheet torso is a tall central plate and two side plates with rounded corners, thin dark gaps around them and bulged faces, dark shoulder pods and two small antennas on the cap; the model back is a flat slab with faint lines. The sheet logo is a gunmetal-edged emblem (a dark shield with a yellow star) on a yellow plate; the model plate is a lighter yellow square. The hood in side view is deeper in the sheet. The sheet block has bevelled corners and a cap box with rounded edges. The eye is a glowing red octagon in a dark recess (the glow comes later from the engine bloom: judge the shape and the recess only).
- rules: BLENDER ROUTE; no Unreal process; no Meshy; no screenshot tool; the modeller edits only part_headtorso.py and part_preview.py (to add a --lit mode with a sun and ambient occlusion, EEVEE, so that the preview looks lit like the sheet); the part keeps its name, pivot, parent and attachment section; the limbs and other parts are not touched.
- editor_state: closed
- token_ceiling: open
- model: opus+sonnet
- tokens: input 278, cache write 663218, cache read 9327197, output 88193
- runs: 9
- milestone: M2
- result: Torso reworked in 9 rounds with an independent critic (opus, read only) from round 1: eye in a dark recess with one solid octagon, company emblem as a texture on the hood top, back as a tower with stepped side plates; 504 tris; the plate on the hood is flush (measured gap 0.1 cm), the side view shows its edge above the crowned hood bevel.

## Relations
- [[task-151-chaser-robot]] — parent global task.
