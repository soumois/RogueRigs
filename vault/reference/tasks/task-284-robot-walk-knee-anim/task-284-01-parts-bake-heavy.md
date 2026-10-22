---
id: task-284-01-parts-bake-heavy
description: "Cut and bake the heavy walk (3d)."
parent: task-284-robot-walk-knee-anim
kind: task
status: done
---
# Cut and bake the heavy walk

- goal: Find the knee joint on the heavy mesh (gap or hinge between thigh and lower-leg panels, checked against the side view of the sheet), cut the game mesh into the 10 rigid parts of the chaser (head-torso, pelvis, 2 upper arms, 2 forearms with hand, fist or cannon, 2 thighs, 2 lower legs with foot; provisional, owner 2026-10-22) with pivots at hip, knee, shoulder and elbow found on the mesh, PartId in UV2, and bake a walk loop with a light knee bend (at most 15 degrees, provisional, owner 2026-10-22) to T_HeavyAnim.exr, with the stride matched to the 180 uu/s speed (heavy 153 uu/s) so the feet do not slide. Generalise meshy_cut_parts.py and meshy_bake_anim.py with a per-robot config instead of copying them.
- agent: 3d
- depends_on: none
- inputs: art-3d-lessons (vault/reference/agents/art-3d-lessons.md), agent-3d (vault/reference/agents/agent-3d.md), robots-meshy-raw-2026-10-22 (vault/reference/tech/pipelines/robots-meshy-raw-2026-10-22.md), art-prompts-robots-meshy (vault/reference/gdd/art-audio/art-prompts-robots-meshy.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), enemy-heavy (vault/reference/gdd/enemies/enemy-heavy.md); files: Art/Meshes/heavy-meshy/heavy_meshy_game.fbx (task-283), sheet vault/assets/heavy-turnaround.webp
- expected_output: Art/Meshes/heavy-meshy/heavy_meshy_parts.fbx (slots kept, robot_eye last), T_HeavyAnim.exr, Art/Meshes/heavy-meshy/heavy_meshy_anim_report.json (joints found with positions, knee height, max knee angle, foot slide cm per cycle, clips, frames), pipeline doc chaser-meshy-pipeline or a new sibling node updated
- done_when: meshy_cut_check passes, PartId bins clean, knee pivot inside the knee panel gap, knee angle within the decided limit, foot slide under 2 cm per cycle, loop seamless; the Orchestrator renders 4 frames of the clip and sends them to the owner; Blender processes stopped
- editor_state: closed (Blender headless only)
- token_ceiling: 60000
- model: sonnet
- tokens: input 252, cache write 1977240, cache read 29452870, output 31423
- runs: 3
- milestone: M2
- result: heavy_meshy_parts.fbx (10 parts, PartId bins exact, 11691 tris) + T_HeavyAnim.exr (20x62, clip 0 walk rows 0-15, rows 16-61 reserved for 285) via generalised meshy_cut_parts/meshy_bake_anim/meshy_cut_check + robot_configs/heavy.json + robot_walk_planted.py; knee max 15 deg, foot slide 0.04 cm/cycle, loop seamless, cut_check 0 anomalies after fix; walk baked for RunRate 1.0 (heavy instances must use 1.0); pelvis 2.5 cm hand-over jump; renders Saved/Renders/task-284/heavy_walk_f0-3.png; doc robot-parts-bake-pipeline.

## Relations
- [[task-284-robot-walk-knee-anim]] — parent global task.
