---
id: asset-pipeline
description: "How assets are made: source images, Meshy generation, hand-modeled pieces, headless Blender cleanup, polygon budgets, height, pivot, rigid-part split, UI icon cut-out tool."
parent: pipelines-index
kind: doc
---
# Asset pipeline

1. Source image: art agent, GPT Image 2 through Kie.ai ([[agent-art]]). Humanoids and bipeds: T-pose or A-pose, front view, plain background, no weapon or effect. Weapons are generated separately and attached to a socket.
2. 3D generation: Meshy image-to-3D in smart-topology mode (`meshy-t2`) for robots, drone and turret ([[agent-3d]]). The miner: owner mesh with Meshy auto-rig (task-382), Mixamo clips made in place (task-386).
3. Hand-modeled: rock blocks, crystals, barrels (faster than AI for simple shapes).
4. Cleanup and decimation on every generated mesh in headless Blender (`Tools/art/blender/cleanup.py`); 3D is the weakest point of AI generation.
5. Robots are generated in one piece, then cut into rigid parts in Blender, each part bound 100 % to one bone ([[enemy-death]]). The miner's plate armor may use the same rigid binding. The parts and bones list per robot: open.
6. Import and Blueprint setup through Unreal MCP (BP agent).

Chaser exception (owner 2026-10-14, replaces the 2026-10-13 rule of a part-by-part Blender model): the chaser uses a Meshy mesh (four-view image to 3D, optimised by Meshy) with its painted base colour texture corrected in Blender (`Tools/art/blender/meshy_texture_fix.py`), then cut into 10 rigid parts for the animation; the owner mixes Blender and Meshy for complex objects. Its two clips are authored in Blender and baked into a texture played by the material (no skeletal mesh at run time). Details: [[enemy-chaser]].

## Mesh values (provisional, owner 2026-10-11)
- Triangle budget: chaser, shooter, kamikaze 3000; heavy 4000; drone 1500; turret 16,000 (owner 2026-10-28, task-346: few turrets on screen; was 10,000, first 2500), extractor (the extraction shuttle, body and four rotors together) 50,000 (owner 2026-10-29, task-366), miner 25,744 as delivered, no decimation (provisional, owner 2026-10-30, task-386), flat faces kept truly flat, no decimation that bulges them; ore chunk pickup 300 target, 500 max per variant (provisional, owner 2026-10-15, task-189); burst rifle 9,892 as delivered, no decimation (provisional, owner 2026-10-30, task-392).
- Height: the collision capsule height of the entity's Blueprint, read by the 3D agent before the run.
- Pivot: bottom centre; hand-held weapons: the pistol grip hand hold (burst rifle, provisional, owner 2026-10-30, task-392).
- Texture (owner 2026-10-17, A1 level 3): painted base colour, normal and roughness maps per asset are allowed, as are shared tiling or world-aligned textures and Meshy textures; the barrel recipe below stays valid. Barrel recipe (owner 2026-10-11, former level 1): flat colours per material from [[color-code]]; wear is a thin lighter band on some edges (geometry, flat material) plus faint scratches from one shared grayscale mask (`Art/Textures/T_WearMask.png`, UV 0, 100 cm per tile) blended toward a lighter tone, on the lid and cap only; stickers and signs are textured decals (alpha: Masked or Translucent in Unreal); a second UV channel `LightMap` without overlap. (Former limits, lifted 2026-10-17: no per-asset painted base texture, no normal or roughness map, Meshy base colour unused.) Blender scripts: `Tools/art/blender/` (barrel, warning_sign, wear_mask, barrel_preview).
- Git: `Art/Source/` and the cleaned FBX are committed; `Art/Meshes/*/raw/` is ignored.

## UI icons
`Tools/art/ui_icon_prepare.js --in <png> --name <name>` (Node, no dependency, PNG codec on built-in zlib) cuts a flat-magenta icon into `Art/UI/Icons/T_Icon_<name>.png`: 256 x 256 RGBA, trimmed, centred, 10 % margin. Alpha = min(smoothstep colour-distance key from the corner colour, magenta colour-difference key), then unmix and despill (no pink fringe), premultiplied area resize. `--keep-aspect` (used for the weapon icons) drops the square canvas: the trimmed content keeps its aspect ratio, the longest side is 256 px and the canvas is the content plus a 4 % margin (of 256) on each side. `--check <png>...` prints size, corner alpha, bbox, margins and remaining magenta-like pixels.

## References
- Miner reference sheet: use front (and back) only; the side view is inconsistent with the T-pose.
- Images: `vault/assets/keyart.jpg`, `enemies-lineup.jpg`, `rigs-lineup.jpg`, `miner-turnaround.jpg`.

## Relations
- [[enemy-chaser]] — the chaser spec: parts and clips, the first robot built part by part.- [[pipelines-index]] — parent index.
- [[color-code]] — colors every asset must follow.
- [[art-prompts]] — verbatim source-image prompts for step 1.
- [[agent-art]] — runs step 1.
- [[agent-3d]] — runs steps 2 and 4.
- [[blender-asset-route]] — the Blender route for simple props (barrel).
- [[barrel-asset]] — the barrel as built.
