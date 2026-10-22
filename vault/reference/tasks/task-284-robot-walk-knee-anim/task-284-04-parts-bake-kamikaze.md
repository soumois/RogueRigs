---
id: task-284-04-parts-bake-kamikaze
description: "Cut and bake the kamikaze leg sway (3d)."
parent: task-284-robot-walk-knee-anim
kind: task
status: done
---
# Cut and bake the kamikaze leg sway

- goal: Cut the kamikaze game mesh into 3 rigid parts (body = head, arms, core shell and vanes; 2 lower legs), pivots at the leg joints found on the mesh, PartId in UV2, and bake a very light leg sway loop (owner 2026-10-22: it hovers, no walk, only a very light leg motion; at most 8 degrees, provisional) to T_KamikazeAnim.exr; the hover bob stays in C++. Generalise meshy_cut_parts.py and meshy_bake_anim.py with a per-robot config instead of copying them.
- agent: 3d
- depends_on: task-284-03-parts-bake-chaser-v2
- inputs: art-3d-lessons (vault/reference/agents/art-3d-lessons.md), agent-3d (vault/reference/agents/agent-3d.md), robots-meshy-raw-2026-10-22 (vault/reference/tech/pipelines/robots-meshy-raw-2026-10-22.md), art-prompts-robots-meshy (vault/reference/gdd/art-audio/art-prompts-robots-meshy.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), enemy-kamikaze (vault/reference/gdd/enemies/enemy-kamikaze.md); files: Art/Meshes/kamikaze-meshy/kamikaze_meshy_game.fbx (task-283), sheet vault/assets/kamikaze-turnaround.webp
- expected_output: Art/Meshes/kamikaze-meshy/kamikaze_meshy_parts.fbx (slots kept, robot_eye last), T_KamikazeAnim.exr, Art/Meshes/kamikaze-meshy/kamikaze_meshy_anim_report.json (joints found with positions, knee height, max knee angle, foot slide cm per cycle, clips, frames), pipeline doc chaser-meshy-pipeline or a new sibling node updated
- done_when: meshy_cut_check passes, PartId bins clean, sway loop seamless; the Orchestrator renders 4 frames of the clip and sends them to the owner; Blender processes stopped
- editor_state: closed (Blender headless only)
- token_ceiling: 60000
- model: sonnet
- tokens: input 60, cache write 107154, cache read 1976250, output 39060
- runs: 1
- milestone: M2
- result: kamikaze_meshy_parts.fbx (3 parts: Body, LowerLeg_L/R = the 2 front legs, separate components, no cut; 5302 tris) + T_KamikazeAnim.exr (20x62, clip 0 sway rows 0-15, legs antiphase 6 deg, RunRate 0.35) via new Tools/art/blender/robot_component_parts_bake.py + robot_configs/kamikaze.json; own check 0 anomalies, PartId exact, seamless; renders Saved/Renders/task-284/kamikaze_sway_f0-3.png.

## Relations
- [[task-284-robot-walk-knee-anim]] — parent global task.
