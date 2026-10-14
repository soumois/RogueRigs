---
id: task-151-04-chaser-refine
description: "Refine the chaser model with the spare triangle budget: calves, hands, block, arms, legs, feet, logo contrast."
parent: task-151-chaser-robot
kind: task
status: done
---
# Chaser: refinement pass

- goal: Refine the chaser model with the spare triangle budget: calves, hands, block, arms, legs, feet, logo contrast.
- agent: 3d
- depends_on: task-151-03-chaser-model
- inputs: asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), mesh-fidelity-loop (vault/reference/tech/pipelines/mesh-fidelity-loop.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), agent-3d (vault/reference/agents/agent-3d.md)
- expected_output: the updated script, FBX, report and renders listed under outputs below
- done_when: criteria C1 to C17 pass in the comparison renders, or the report lists the criteria still failing after 5 returns; total tris at most 3000 (about 2600 aimed); the script, the FBX and the report committed by name; nothing imported in Unreal
- context: step 03 (commit 3eefd05) built the ten parts with 1632 triangles; the owner wants a refinement pass that uses the spare budget (target about 2600 triangles, hard ceiling 3000) before the animation freezes the parts, and he reports a problem with the calves and the hands. The Orchestrator's reading of the problem, from the compare renders (Art/Meshes/chaser/compare_front.png, compare_side.png, compare_back.png):
  - calves: from the side the ShinFoot is a thin yellow box with a separate small yellow block floating at the ankle and a dark bar behind it; the sheet has ONE continuous tapering shin guard from the knee to the ankle (about twice as deep front to back as the model), a dark octagonal ankle joint above the foot, and the dark leg bar behind it;
  - hands: the model hands are rectangular dark mitts with three straight finger blocks hanging; the sheet hands are curled claws: a narrower palm, three fingers hooking inward and forward, and a thumb.
  The owner has not said more: report in your result what you changed on the calves and the hands and why, so he can correct the reading.
- images: the same sheet and crops as step 03 (vault/assets/chaser-turnaround.png, chaser-front.png, chaser-side.png, chaser-back.png) and the existing renders; the limbs stay as thick as drawn (do not slim them); the part list, names, pivots, hierarchy, joints (Pelvis root; HeadTorso waist; UpperArm shoulder; ForearmHand elbow; Thigh hip; ShinFoot knee) and the 10 objects do not change.
- start from the committed Tools/art/blender/chaser_model.py and Art/Meshes/chaser/SM_Chaser_parts.report.json (keep its fidelity object and add a refinement object); criteria C1 to C10 of step 03 must keep passing (C9: no panel lines or bolts; the triangle ceiling is now a target of about 2600 and a ceiling of 3000).
- max_returns: 5 (new count for this step); same strictness rule as step 03.
- added criteria (judged on the renders against the crops, same cameras as step 03):
  - C11 Calves: each ShinFoot has ONE continuous tapering yellow shin guard from the knee to the ankle, about as deep front to back as the sheet shows (side view), a dark octagonal ankle joint between the shin and the foot, no floating block at the ankle; the dark leg bar visible behind the shin in front view as on the sheet.
  - C12 Hands: each ForearmHand ends in a curled claw: a narrower palm, three fingers hooking inward and forward, a thumb; three to four finger segments per finger at most; the hand silhouette in the front crop and in the side crop matches the sheet (hands curl toward the thigh in side view).
  - C13 Block: the HeadTorso gets the facets of the sheet: the hood front with a chamfered top edge, a rounded shoulder line, and the back with several plates (at least three large plates, front view of the back crop); the cap box stays; the face recess and the eye stay.
  - C14 Arms: rounded (chamfered) upper arms like the sheet, a wider shoulder cap, forearms with a guard plate; elbow bent as before.
  - C15 Legs: thigh and shin guards faceted like the sheet; the legs splay slightly outward as measured on the crop (hip about 9 % of H, knee about 11 %, foot about 15 % of H from the centre line); the hips get the small guard plates of the sheet instead of plain boxes.
  - C16 Feet and logo: the feet are wider with two angled toe blocks and a heel block, about 23 % of H long, 15 % tall at the heel; the logo plate gets a visible contrast with the block (a larger dark glyph on a plate of a slightly different yellow tone, or a dark border; keep the four material slots: use the logo texture to carry the contrast).
  - C17 Budget and style: total triangles about 2600 (at most 3000); flat shading; no cylinders except the knee and ankle prisms.
- outputs: the same files updated (Tools/art/blender/chaser_model.py, Art/Meshes/chaser/SM_Chaser_parts.fbx, SM_Chaser_parts.report.json with the refinement object: returns_used, each criterion C1 to C17 verdict and evidence, silhouette differences, tris per part, what changed on the calves and the hands), new renders in Art/Meshes/chaser/ (not committed), including the compare images for front, side and back and a close-up render of one hand and one calf (ortho, two views each) opened and judged.
- editor_state: closed
- token_ceiling: open
- model: sonnet
- tokens: input 70, cache write 142982, cache read 2886985, output 15966
- runs: 1
- milestone: M2
- result: Refinement of the monolithic model: 2056 tris, calves and hands rebuilt, C13 (back plates) still failing; saved as git tag backup-chaser-monolithic; owner chose the part-by-part rework next.

## Relations
- [[task-151-chaser-robot]] — parent global task.
