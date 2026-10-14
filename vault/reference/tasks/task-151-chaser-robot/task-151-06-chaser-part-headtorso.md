---
id: task-151-06-chaser-part-headtorso
description: "Rebuild the chaser HeadTorso alone against its close-up crops of the validated sheet."
parent: task-151-chaser-robot
kind: task
status: done
---
# Chaser part: HeadTorso

- goal: Rebuild the chaser HeadTorso alone against its close-up crops of the validated sheet.
- agent: 3d
- depends_on: task-151-05-chaser-split
- inputs: asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), mesh-fidelity-loop (vault/reference/tech/pipelines/mesh-fidelity-loop.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), agent-3d (vault/reference/agents/agent-3d.md)
- expected_output: Tools/art/blender/chaser/part_headtorso.py updated, Art/Meshes/chaser/parts/headtorso.report.json, renders in Art/Meshes/chaser/parts/
- done_when: All criteria of this part pass in the part renders against the crops, or the report lists the criteria still failing after 5 returns; at most 600 triangles; the part module and its report committed by name
- context: the chaser is rebuilt part by part (owner 2026-10-13, after the monolithic model of commits 3eefd05 and bc01b6b, saved as git tag backup-chaser-monolithic and in Art/Meshes/chaser/backup-monolithic/). Each unique part has its own module and is judged alone against close-up crops of the validated sheet; the left and right parts are mirrors.
- images: the validated sheet vault/assets/chaser-turnaround.png and its three views chaser-front.png, chaser-side.png, chaser-back.png; the close-up crops of this part at double scale in Art/Source/chaser/part-crops/<part>-front.png, -side.png, -back.png (the crop windows are rough: they may include a neighbouring part; judge only your part).
- contract (do not change): the part name, its pivot at the joint position, its parent in the hierarchy and the attachment section at the joint (read the `joints` and `hierarchy` and the per-part bounds in Art/Meshes/chaser/SM_Chaser_parts.report.json); the limbs stay as thick as drawn on the sheet; four material slots in the order yellow_body, gunmetal, eye_red, logo_plate (slots a part does not use stay empty but keep their index); flat shading; no bolts, cables or panel lines; units cm; forward is +X, left is +Y.
- loop: mesh-fidelity-loop with max_returns 5 and the strictness rule (a criterion passes only if someone comparing the render and the crop side by side would not call that part different; name what still differs even where you pass). Render the part alone with `part_preview.py --only <part>`: orthographic front, side and back and one close-up of the hardest detail, on a light grey background at the scale of the part crops; open each render with its crop (Read) and judge each criterion with evidence measured from the images (fractions of the part height).
- rules: edit ONLY your part module and its report; do not touch the other parts, common.py or assemble.py except to report a needed change (say what in the report); no Unreal process, no Meshy, no screenshot or viewport tool; the pre-commit hook may regenerate vault/_paths.md: do not commit it.
- outputs: Tools/art/blender/chaser/part_<part>.py updated, Art/Meshes/chaser/parts/<part>.report.json (tris, bounds, pivot and attachment check, fidelity object with returns_used and each criterion verdict and evidence, what still differs), renders in Art/Meshes/chaser/parts/ (not committed); commit by name with message `task-151-<step>-<part>: <summary>` and the Co-Authored-By line.
- part: headtorso (HeadTorso)
- fidelity_criteria:
  - C1 Block: one chunky block about 34 % of H tall, 24 % wide, deep hood in side view with a chamfered top edge, a small cap box on top.
  - C2 Face: dark recess with the red emissive octagon eye in the lower half of the front face, logo plate above it with its dark frame and glyph (keep the lighter yellow plate).
  - C3 Shoulders: a three-step rounded shoulder line as on the sheet front view.
  - C4 Back (the known failing point C13 of the monolithic model): three large back plates that READ in the back view without ambient occlusion: separate them by a visible reveal (a step of at least 0.6 cm between plates and a face angle difference of at least 25 degrees), as the back crop shows.
  - C5 Budget: at most 600 triangles (168 today), flat shading.
- editor_state: closed
- token_ceiling: open
- model: sonnet
- tokens: input 24, cache write 24778, cache read 255129, output 3127
- runs: 1
- milestone: M2
- result: HeadTorso rebuilt: stepped side back plates (1.9 cm reveal, about 35 degrees), 176 tris; C4 back plates still low contrast in flat light (3 returns).

## Relations
- [[task-151-chaser-robot]] — parent global task.
