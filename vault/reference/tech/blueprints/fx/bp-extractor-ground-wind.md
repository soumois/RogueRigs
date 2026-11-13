---
id: bp-extractor-ground-wind
description: "NS_ExtractorGroundWind and M_ExtractorGroundWind: textured beige ground wind puffs, streaks and dots under the extractor; open for material, atlas UV, edge fix."
parent: blueprints-fx-index
kind: doc
---
# NS_ExtractorGroundWind

- asset_path: /Game/RogueRigs/FX/GroundWind/NS_ExtractorGroundWind (Niagara system, no Blueprint); material M_ExtractorGroundWind; instances MI_GroundWind_Disc, MI_GroundWind_Puffs, MI_GroundWind_Ring, MI_GroundWind_Streaks; textures T_GroundWind_Disc (512), T_GroundWind_Puffs, T_GroundWind_Ring, T_GroundWind_Streaks (1024), all in the same folder.
- parent_class: UNiagaraSystem
- purpose: the ground wind on the floor under the extractor at arrival and exit: the beige puffs, streaks and dot clusters of the owner's image 2 ([[task-542-ground-particle-edges]]). Partial state of the cancelled task-466: textures, material and instances in f0381dfa, system and its component on [[bp-extraction-shuttle]] in c39d5f87, kept as is by the owner.
- emitters: open (not read back in task-542).
- material: M_ExtractorGroundWind, translucent, unlit, two-sided, after DOF, used with Niagara sprites. A Custom node builds the sample UV from TexCoord, Particle Random, `AtlasCells` (N) and `FlipV` (F): cell = floor(R x N x N) in an N x N atlas, uv flipped in V when F > 0.5, result (uv + cell) / N into the `Shape` texture parameter. Emissive = Shape x ParticleColor, Opacity = Shape x ParticleColor.
- instances: Disc and Ring AtlasCells 1, Puffs and Streaks AtlasCells 2 (2 x 2 atlas); FlipV 0 on all; each sets `Shape` to its texture.
- textures: compression Default, sRGB on, address Wrap, mips from the Effects group. Sources Art/Source/extractor-fx/ground-wind/*.png, made by Tools/art/make_ground_wind_sprites.js; alpha 0 on the border of each image and, in the atlases, 16 px or more around each cell.
- edge_fix (task-542, 80bd4cad): the sprites showed dotted lines on the four quad edges. Cause: the Custom node took `frac(UV)`; at the sprite border the interpolated UV crosses 0 or 1 inside a 2 x 2 pixel quad, frac jumps, the UV derivative explodes and the GPU samples the smallest mip, an average beige with non-zero alpha. Fix: `saturate(UV)` instead of `frac(UV)`, identical inside the quad, so colour, size, timing and count are unchanged; it applies to all four instances. Checked by -game capture of the arrival takeoff at 1920 x 1080: edges on every puff before, none after, same puffs.
- colour: beige, no code colour ([[color-code]]).
- components: none (Niagara asset). logic_in_bp: none. Wiring in [[bp-extraction-shuttle]]: open (component name and parameters not read back).
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[bp-extraction-shuttle]] — the Blueprint that plays it.
- [[bp-extractor-rotor-wash]] — the rotor wash under the same extractor.
- [[task-542-ground-particle-edges]] — the task that fixed the sprite edges.
- [[color-code]] — colour rules.
