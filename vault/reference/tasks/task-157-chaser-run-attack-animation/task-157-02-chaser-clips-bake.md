---
id: task-157-02-chaser-clips-bake
description: "Author the run loop and attack clip on the ten parts and bake one transform per part and frame into a float texture."
parent: task-157-chaser-run-attack-animation
kind: task
status: todo
---
# Chaser: run and attack clips baked

- goal: Author the run loop and attack clip on the ten parts and bake one transform per part and frame into a float texture.
- agent: 3d
- depends_on: task-157-01-chaser-run-attack-animation
- inputs: enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), chaser-model-pipeline (vault/reference/tech/pipelines/chaser-model-pipeline.md), agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md)
- expected_output: The headless script Tools/art/blender/meshy_bake_anim.py reads Art/Meshes/chaser-meshy/chaser_meshy_parts.fbx, keys two clips on the part pivots procedurally (no hand-posed file): RUN, a 16 frame loop at 30 fps (0.533 s, left and right legs in opposition, knees bending, arms swinging opposite to the legs with the elbows bent about 40 degrees, torso leaning 8 degrees forward with a small vertical bob of the pelvis, feet not sliding in a believable cycle for a speed of 180 cm per second, first frame equals the frame after the last), and ATTACK, 15 frames at 30 fps (0.5 s, not a loop): 3 frames of wind-up (0.1 s: right arm pulled back and up, torso leaning back 10 degrees), 4 frames of strike (the arm swinging forward and down, the torso lunging forward 15 degrees with a half step), 8 frames of recovery back to the rest pose; then bakes, for each frame and each part, the rotation quaternion (x, y, z, w) and the translation offset t = T + p - R p (so the material computes new_position = R v + t with no pivot data) into Art/Meshes/chaser-meshy/T_ChaserAnim.exr (RGBA float16, 20 texels wide = part index x 2 + 0 for the quaternion and + 1 for the translation in cm in rgb, 31 texels tall = frames: run frames 0 to 15 then attack frames 16 to 30; quaternion signs kept continuous between frames); the report anim_report.json (frame rate, clip start frame and length, part order and index, the formula, the max rotation angle and translation per clip, texture size); a verification inside the script: applying the baked quaternions and translations to the rest mesh vertices must equal the keyed pose within 0.05 cm for all frames (report the max error); a contact sheet PNG per clip (8 frames of the run from the side, 8 frames of the attack from the side, lit as in meshy_texture_fix.py renders) in Art/Meshes/chaser-meshy/renders/anim/
- done_when: The script runs headless; the EXR exists with the stated size and the report states the formula and the indices; the bake verification error is at most 0.05 cm; the run loop closes (frame 0 equals frame 16 within the same tolerance); the contact sheets exist; the agent states facts and numbers and gives no quality verdict (the Orchestrator judges from the contact sheets); nothing imported in Unreal; the script and the reports committed by name, EXR and renders untracked
- rules: BLENDER ROUTE; no Unreal process; commit by name with message `task-157-02-chaser-clips-bake: <summary>`
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-157-chaser-run-attack-animation]] — parent global task.
