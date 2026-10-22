---
id: task-284-03-parts-bake-chaser-v2
description: "Cut and bake the chaser v2 walk (3d)."
parent: task-284-robot-walk-knee-anim
kind: task
status: done
---
# Cut and bake the chaser v2 walk

- goal: Find the knee joint on the chaser v2 mesh (gap or hinge between thigh and lower-leg panels, checked against the side view of the sheet), cut the game mesh into the 10 rigid parts of the chaser (head-torso, pelvis, 2 upper arms, 2 forearms with hand, fist or cannon, 2 thighs, 2 lower legs with foot; provisional, owner 2026-10-22) with pivots at hip, knee, shoulder and elbow found on the mesh, PartId in UV2, and bake the run loop and the attack clip of chaser v1 (same clip ids, frames and wind-up) with a light knee bend (at most 15 degrees, provisional, owner 2026-10-22) to T_ChaserV2Anim.exr, with the stride matched to the 180 uu/s speed (heavy 153 uu/s) so the feet do not slide. Generalise meshy_cut_parts.py and meshy_bake_anim.py with a per-robot config instead of copying them.
- agent: 3d
- depends_on: task-284-02-parts-bake-shooter
- inputs: art-3d-lessons (vault/reference/agents/art-3d-lessons.md), agent-3d (vault/reference/agents/agent-3d.md), robots-meshy-raw-2026-10-22 (vault/reference/tech/pipelines/robots-meshy-raw-2026-10-22.md), art-prompts-robots-meshy (vault/reference/gdd/art-audio/art-prompts-robots-meshy.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md); files: Art/Meshes/chaser-v2-meshy/chaser_v2_meshy_game.fbx (task-283), sheet vault/assets/chaser-turnaround-v2.webp
- expected_output: Art/Meshes/chaser-v2-meshy/chaser_v2_meshy_parts.fbx (slots kept, robot_eye last), T_ChaserV2Anim.exr, Art/Meshes/chaser-v2-meshy/chaser_v2_meshy_anim_report.json (joints found with positions, knee height, max knee angle, foot slide cm per cycle, clips, frames), pipeline doc chaser-meshy-pipeline or a new sibling node updated
- done_when: meshy_cut_check passes, PartId bins clean, knee pivot inside the knee panel gap, knee angle within the decided limit, foot slide under 2 cm per cycle, loop seamless; the Orchestrator renders 4 frames of the clip and sends them to the owner; Blender processes stopped
- editor_state: closed (Blender headless only)
- token_ceiling: 60000
- model: sonnet
- tokens: input 106, cache write 113512, cache read 3495022, output 41810
- runs: 1
- milestone: M2
- result: chaser_v2_meshy_parts.fbx (10 parts, 6155 tris, PartId exact, cut_check 0 anomalies) + T_ChaserV2Anim.exr (20x31, v1 layout: clip 0 run rows 0-15, clip 1 attack rows 16-30, wind-up 16-18); run baked for RunRate 1.5 (0.7 infeasible with 15 deg knee at 180 uu/s); knee max 15 deg, slide 0.03 cm/cycle, seamless; foot dips -1.36 cm at pivot switch; renders Saved/Renders/task-284/chaser_v2_run_f0-3, chaser_v2_attack_f0-1.png.

## Relations
- [[task-284-robot-walk-knee-anim]] — parent global task.
