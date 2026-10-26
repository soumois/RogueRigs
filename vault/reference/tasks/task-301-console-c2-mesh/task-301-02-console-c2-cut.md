---
id: task-301-02-console-c2-cut
description: "Cut the console C2 mobile head and set its material slots (3d): console c2 uplink mast mesh, step 02."
parent: task-301-console-c2-mesh
kind: task
status: done
---
# Cut the console C2 mobile head and set its material slots

- goal: Blender route from the owner Meshy FBX: separate the mobile part (dish + head above the mast elbow, image 1) into its own mesh with its pivot on the vertical mast axis; split the materials into three slots: AntennaOrange (orange parts of the antenna head, the blinking one), OtherOrange (every other orange part), Screen (the slanted console screen); base colour texture kept for the rest; FBX_SCALE_UNITS export; report with sizes, pivot and slot face counts; preview renders (top-down and side, fixed camera and light).
- agent: 3d
- depends_on: task-301-01-console-c2-files
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), mesh-fidelity-loop (vault/reference/tech/pipelines/mesh-fidelity-loop.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), bp-objective-console (vault/reference/tech/blueprints/world/bp-objective-console.md); files: Art/Source/console-meshy/meshy/Meshy_AI_Satellite_Receiver_St_1023222045_texture.fbx and its maps; Art/Source/console-meshy/owner/ref1-mobile-part.png; vault/assets/console-c2-turnaround.webp
- expected_output: Art/Source/console-meshy/clean/console_c2_base.fbx and console_c2_head.fbx, cleanup script, renders, report
- done_when: two FBX files; head pivot on the mast axis (numbers in the report); three named slots; renders sent to the owner by the Orchestrator; no Blender process left running; committed
- editor_state: any
- token_ceiling: 40000
- model: sonnet
- tokens: input 62, cache write 118263, cache read 2187631, output 55376
- usage: turns 31, context start 10k peak 122k, resumes 0, 13 min, units 0.64 M
- runs: 1
- milestone: M2
- result: Blender route: console_c2_base.fbx (147.7 x 126.7 x 108 cm) and console_c2_head.fbx (dish, feed arm, joint; cut plane z 108) in Art/Source/console-meshy/clean/; head origin on the mast axis, offset on base (-4.904, 16.690, 108.0) cm; slots ConsoleBody, AntennaOrange (head 332 tris), OtherOrange (base 3778 tris), Screen (10 tris); FBX_SCALE_UNITS; fixed-camera renders in clean/renders/ (assembled, lifted, tint, source_raw); commit d996ab57.

## Relations
- [[task-301-console-c2-mesh]] — parent global task.
