---
id: task-151-03-chaser-model
description: "Model the chaser part by part in Blender from the validated sheet, judged by the fidelity loop."
parent: task-151-chaser-robot
kind: task
status: in-progress
---
# Chaser: model the 10 parts

- goal: Model the chaser part by part in Blender from the validated sheet, judged by the fidelity loop.
- agent: 3d
- depends_on: task-151-02-chaser-sheet-simplified
- inputs: asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), mesh-fidelity-loop (vault/reference/tech/pipelines/mesh-fidelity-loop.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), bp-chaser (vault/reference/tech/blueprints/enemies/bp-chaser.md), agent-3d (vault/reference/agents/agent-3d.md)
- expected_output: the ten part objects, the report and the renders listed under outputs below
- done_when: all criteria C1 to C10 pass in the comparison renders, or the report lists the criteria still failing after 5 returns; total tris at most 3000; the script, the FBX and the report committed by name; nothing imported in Unreal
- images (validated by the owner 2026-10-13, limbs kept as drawn): vault/assets/chaser-turnaround.png (the sheet: front, side, back), crops vault/assets/chaser-front.png (550 x 790), chaser-side.png (320 x 790), chaser-back.png (560 x 790); the robot stands about 715 to 735 px tall in each crop; the owner kept the limbs as thick as drawn.
- scope: modelling only (no rig, no animation, no Unreal): a neutral standing pose, units cm, Blender route.
- parts (10, each its own object, named, pivot at the joint): HeadTorso, Pelvis, UpperArm_L, UpperArm_R, ForearmHand_L, ForearmHand_R, Thigh_L, Thigh_R, ShinFoot_L, ShinFoot_R. Hierarchy for the report: Pelvis is the root; HeadTorso at the waist; UpperArm at the shoulder (child of HeadTorso); ForearmHand at the elbow (child of UpperArm); Thigh at the hip (child of Pelvis); ShinFoot at the knee (child of Thigh). Report the joint positions in cm.
- budget: 3000 triangles in total; guide: HeadTorso 600, Pelvis 120, each UpperArm 150, each ForearmHand 300, each Thigh 150, each ShinFoot 300 (a ceiling, not a target).
- height: the collision capsule height of BP_Chaser (read bp-chaser first and report the value you used for the model height from the sole to the top of the head block).
- materials (flat colour per material, charter A1): yellow_body (company yellow, about #F5BA20; the sheet shows #FEC630 lit and #D7A21F in shade), gunmetal (about #3A3A3A), eye_red (emissive red, the octagon slot), logo_plate (yellow plate with a dark gunmetal glyph as a textured decal, as on the sheet chest); no per-asset painted base texture; flat shading.
- render: orthographic front, side and back cameras on a light grey background at the scale of the crops, plus one three-quarter view; each judged against the matching crop.
- fidelity_criteria (judged on the renders against the crops; proportions are fractions of the full height H, tolerance 15 % of the value unless stated; max_returns 5; strictness: a criterion passes only if someone comparing the render and the crop side by side would not call that part different):
  - C1 Block: HeadTorso is one chunky block, about 34 % of H tall, about 24 % of H wide in front view, about 29 % of H deep in side view; a small top cap box on top; in side view the front face leans like a hood (the face with the eye is at the front, the block extends backwards and is deeper at the top).
  - C2 Face: on the front face of the block, a dark rectangular recess about 45 % of the block width, in the lower half of the block, holding the red emissive octagon eye; above it the logo plate (yellow plate, dark glyph, about 20 % of the block width) centred.
  - C3 Pelvis: a dark gunmetal block under the block, about 14 % of H wide and 10 % of H tall, narrower than the block, with a yellow lower rim visible in front view.
  - C4 Arms: shoulder-to-shoulder (outer edges) about 69 % of H; each arm = yellow upper arm (about 12 % of H wide, a faceted box wider at the top), a dark elbow connector, a yellow forearm (about 10 % of H wide), a dark hand with three chunky finger blocks; elbows slightly bent, hands end at about 55 % of H down from the top, slightly forward in side view.
  - C5 Legs: from the pelvis to the sole about 60 % of H; each leg = yellow thigh box (about 12 % of H wide in front view), a dark octagonal knee prism, a yellow shin box (about 8 % of H wide), the dark leg bars visible between the yellow boxes in front view; legs straight and parallel, feet about 45 % of H apart (outer edges).
  - C6 Feet: yellow foot with a dark sole and a dark toe block, about 25 % of H long and 12 % of H tall in side view, pointing forward.
  - C7 Colours and materials: yellow on the block, arms, thighs, shins and feet; gunmetal on the pelvis, joints, hands, soles and leg bars; red emissive only on the eye; the four material slots as listed; no other colour.
  - C8 Parts and pivots: exactly 10 separate objects with the listed names, each with its pivot at its joint, the hierarchy and joint positions in the report; parts do not overlap each other at the joints by more than 1 cm.
  - C9 Style: large flat facets visible, flat shading, no small panel lines, bolts or cables, no cylinders except the knee prisms; total triangles at most 3000.
  - C10 Silhouette: the filled silhouette of the render in each orthographic view matches the silhouette of the crop (same position of the head top, hands, knees and soles as fractions of H); report the largest difference in percent of H per view (pass when at most 5 %).
- outputs: Tools/art/blender/chaser_model.py (bpy, headless: see blender-asset-route), Art/Meshes/chaser/SM_Chaser_parts.fbx (the 10 objects, cm, pivots at joints), Art/Meshes/chaser/SM_Chaser_parts.report.json (tris per part and total, bounds, joints, hierarchy, materials, a fidelity object with returns_used, each criterion verdict and evidence, final render paths), the final renders in Art/Meshes/chaser/ (not committed).
- rules: flat shading; UV0 for the logo decal only; a LightMap UV1 without overlap for every part as in blender-asset-route; no Meshy, no Unreal, no screenshot tool.
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-151-chaser-robot]] — parent global task.
