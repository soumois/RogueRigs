---
id: task-155-01-chaser-meshy-texture-fixes
description: "Correct the texture of the optimised Meshy chaser: shoulder-to-torso joints fully dark, and the hood logo replaced by the company emblem (yellow on black, smooth)."
parent: task-155-chaser-meshy-texture-fixes
kind: task
status: in-progress
---
# Chaser Meshy: texture fixes

- goal: Correct the texture of the optimised Meshy chaser: shoulder-to-torso joints fully dark, and the hood logo replaced by the company emblem (yellow on black, smooth).
- agent: 3d
- depends_on: none
- inputs: blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), agent-3d (vault/reference/agents/agent-3d.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), mesh-fidelity-loop (vault/reference/tech/pipelines/mesh-fidelity-loop.md)
- expected_output: In Art/Meshes/chaser-meshy/: the corrected texture (PNG, same size and same UV as the original, the geometry untouched) and an FBX that uses it, a Blender script Tools/art/blender/meshy_texture_fix.py (headless, repeatable), before and after lit renders of the shoulder-to-torso joints (both sides, front and three-quarter close-ups), of the hood top with the logo (close-up) and of the whole robot front, side, back; a report JSON (faces and texels recoloured, the logo placement and size)
- done_when: The mesh and its UV are identical to the input (same vertex and face counts); the joint faces between each shoulder and the torso read fully dark with no yellow mix left; the rest of the texture is unchanged (the report gives the number of texels changed outside the two fixes: zero); the hood logo is the company emblem from Art/Meshes/chaser/T_ChaserLogo.png redrawn yellow on a black plate (black background, yellow rock outline and KV letters), with clean smooth edges at 512 px or more of source resolution; the script runs headless; nothing imported in Unreal; the script and the report committed by name, the FBX, textures and renders left untracked until the owner decides
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-155-chaser-meshy-texture-fixes]] — parent global task.
