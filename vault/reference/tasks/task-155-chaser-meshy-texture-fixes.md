---
id: task-155-chaser-meshy-texture-fixes
description: "Correct the optimised Meshy chaser texture: dark shoulder-to-torso joints, and the company emblem as hood logo."
parent: tasks-done-index
kind: task
status: done
---
# Chaser Meshy: texture fixes

- request: The owner received the optimised Meshy chaser (3115 triangles, one object, one 2048 px texture) and asks two texture corrections, everything else being good: the joints between the shoulders and the torso mix dark and yellow and must be only dark, and the logo must change while keeping its black background with a yellow logo, smoothed because it is a texture (owner, 2026-10-14) (owner, 2026-10-14)
- goal: Correct the texture of the optimised Meshy chaser: shoulder-to-torso joints fully dark, and the hood logo replaced by the company emblem (yellow on black, smooth).
- acceptance: The mesh and its UV are identical to the input (same vertex and face counts); the joint faces between each shoulder and the torso read fully dark with no yellow mix left; the rest of the texture is unchanged (the report gives the number of texels changed outside the two fixes: zero); the hood logo is the company emblem from Art/Meshes/chaser/T_ChaserLogo.png redrawn yellow on a black plate (black background, yellow rock outline and KV letters), with clean smooth edges at 512 px or more of source resolution; the script runs headless; nothing imported in Unreal; the script and the report committed by name, the FBX, textures and renders left untracked until the owner decides; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-14).
- confirmed_by_owner: yes (2026-10-14)
- milestone: M2
- next_step: none
- result: Done. texture fixed (joints dark, emblem logo); logo small and old relief ghost remain; untracked outputs in Art/Meshes/chaser-meshy/

## Subtasks
1. task-155-01-chaser-meshy-texture-fixes — 3d

## Relations
- [[tasks-done-index]] — parent index.
- [[task-155-01-chaser-meshy-texture-fixes]] — step 1, 3d.
