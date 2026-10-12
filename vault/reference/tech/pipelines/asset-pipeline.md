---
id: asset-pipeline
description: "How 3D assets are made: source images, Meshy generation, hand-modeled terrain pieces, headless Blender cleanup, polygon budgets, height, pivot, rigid-part split."
parent: pipelines-index
kind: doc
---
# Asset pipeline

1. Source image: art agent, GPT Image 2 through Kie.ai ([[agent-art]]). Humanoids and bipeds: T-pose or A-pose, front view, plain background, no weapon or effect. Weapons are generated separately and attached to a socket.
2. 3D generation: Meshy image-to-3D in smart-topology mode (`meshy-t2`) for robots, drone and turret ([[agent-3d]]). The miner (auto-rig, Tripo) is deferred to its own task.
3. Hand-modeled: rock blocks, crystals, barrels (faster than AI for simple shapes).
4. Cleanup and decimation on every generated mesh in headless Blender (`Tools/art/blender/cleanup.py`); 3D is the weakest point of AI generation.
5. Robots are generated in one piece, then cut into rigid parts in Blender, each part bound 100 % to one bone ([[enemy-death]]). The miner's plate armor may use the same rigid binding. The parts and bones list per robot: open.
6. Import and Blueprint setup through Unreal MCP (BP agent).

## Mesh values (provisional, owner 2026-10-11)
- Triangle budget: chaser, shooter, kamikaze 3000; heavy 4000; drone 1500; turret 2500.
- Height: the collision capsule height of the entity's Blueprint, read by the 3D agent before the run.
- Pivot: bottom centre.
- Texture (owner 2026-10-11, level 1, validated on the barrel): flat colours per material from [[color-code]]; wear is a thin lighter band on some edges (geometry, flat material) plus faint scratches from one shared grayscale mask (`Art/Textures/T_WearMask.png`, UV 0, 100 cm per tile) blended toward a lighter tone, on the lid and cap only; stickers and signs are textured decals (alpha: Masked or Translucent in Unreal); a second UV channel `LightMap` without overlap. No per-asset painted base texture, no normal or roughness map. Meshy base color texture is not used. Blender scripts: `Tools/art/blender/` (barrel, warning_sign, wear_mask, barrel_preview).
- Git: `Art/Source/` and the cleaned FBX are committed; `Art/Meshes/*/raw/` is ignored.

## References
- Miner reference sheet: use front (and back) only; the side view is inconsistent with the T-pose.
- Images: `vault/assets/keyart.jpg`, `enemies-lineup.jpg`, `rigs-lineup.jpg`, `miner-turnaround.jpg`.

## Relations
- [[pipelines-index]] — parent index.
- [[color-code]] — colors every asset must follow.
- [[art-prompts]] — verbatim source-image prompts for step 1.
- [[agent-art]] — runs step 1.
- [[agent-3d]] — runs steps 2 and 4.
