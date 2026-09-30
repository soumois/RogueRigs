---
id: asset-pipeline
description: "How 3D assets are made: source images, Meshy and Tripo generation, hand-modeled terrain pieces, cleanup, rigid-part split in Blender."
parent: pipelines-index
kind: doc
---
# Asset pipeline

1. Source image (art agent prompts, ChatGPT images). Humanoids and bipeds: T-pose or A-pose, front view, plain background, no weapon or effect. Weapons are generated separately and attached to a socket.
2. 3D generation: Meshy 7 (low-poly mode) for robots, drone and turret; Tripo for the miner (auto-rig).
3. Hand-modeled: rock blocks, crystals, barrels (faster than AI for simple shapes).
4. Cleanup and decimation on every generated mesh; 3D is the weakest point of AI generation.
5. Robots are generated in one piece, then cut into rigid parts in Blender (Blender MCP), each part bound 100 % to one bone ([[enemy-death]]). The miner's plate armor may use the same rigid binding.
6. Import and Blueprint setup through Unreal MCP (BP agent).

- Miner reference sheet: use front (and back) only; the side view is inconsistent with the T-pose.
- References: `vault/assets/keyart.jpg`, `enemies-lineup.jpg`, `rigs-lineup.jpg`.

## Relations
- [[pipelines-index]] — parent index.
- [[color-code]] — colors every asset must follow.
