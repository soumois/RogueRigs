---
id: bp-space-background-material
description: "M_SpaceBackground and its instances MI_Space_Stars, Nebula, Dust: procedural unlit space void material, parameter table, blend modes, cost, tuning and revert; open to tune it."
parent: blueprints-world-index
kind: doc
---
# M_SpaceBackground

- asset_path: /Game/RogueRigs/Environment/Space/M_SpaceBackground; instances MI_Space_Stars, MI_Space_Nebula, MI_Space_Dust in the same folder (task-332-01, provisional). MI_Space_Nebula is no longer on any plane since task-335-04 (NebulaPlane uses MI_Space_Clouds, see [[bp-space-texture-material]]); the asset stays.
- parent_class: UMaterial (instances: MaterialInstanceConstant of the master)
- purpose: procedural space behind the arena (stars, nebula, dust) with no texture; drawn by [[bp-space-background]].
- components: not applicable (material).
- exposed_defaults: see the parameter table below.
- events: none.
- logic_in_bp: none. Graph: Unlit, TwoSided, emissive only, no world position offset, no texture. One Custom HLSL node `SpaceLayer`: sin-hash value noise on world XY (same family as M_FloorPlates, [[bp-floor-plates-material]]). LayerMode 1 = nebula (3 octaves, slow flow by Time, 3-hue ramp ColorA/B/C driven by a 4th low-frequency noise). LayerMode 0 and 2 = stars/dust (thresholded cell noise, random star position, size and brightness per cell, sine twinkle, drift by FlowSpeed).
- blend modes: set per instance by BasePropertyOverrides: stars Opaque, nebula Additive, dust Additive. One master.
- assets: no texture, no mesh.
- owner_agent: agent-bp
- cost: estimate only, points branch about 50 to 60 ALU, nebula about 120 to 150 ALU, no texture fetch, uniform branch on LayerMode.
- look_rule: the player-light mask ([[mpp-player-light-mask]]) multiplies the background by 0.5 far from the miner since task-335-05 (void pixels, Z below -200); it was about 0.08 (Q5 of [[task-332-space-background]]).
- tuning: Intensity of the nebula and dust instances is the knob (see finding). Values are provisional (Q3, Q4).
- revert: delete the BP_SpaceBackground actor from L_ArenaM1; the material alone has no effect on other assets.

## Parameters (master default / Stars / Nebula / Dust)
| Parameter | Master | Stars | Nebula | Dust |
|---|---|---|---|---|
| Intensity | 1 | 1 | 1.6 (was 0.06, task-333-01, 975425cf) | 0.06 |
| LayerMode | 0 | 0 | 1 | 2 |
| Scale (uu) | 500 | 500 | 4000 | 400 |
| Density | 0.15 | 0.15 | 0.5 | 0.12 |
| StarSize (max radius uu) | 40 | 40 | unused | 25 |
| TwinkleSpeed (Hz) | 0.25 | 0.25 | unused | 0.05 |
| FlowSpeed (uu/s) | 0 | 0 | 15 | 40 |
| Contrast | 2.5 | unused | 2.5 | unused |
| ColorA | (0.85, 0.92, 1) | same | (0.16, 0.13, 0.42) | (0.45, 0.40, 0.60) |
| ColorB | (1, 0.92, 0.8) | same | (0.36, 0.20, 0.55) | (0.60, 0.45, 0.55) |
| ColorC | (0.5, 0.26, 0.44) | unused | (0.52, 0.26, 0.44) | unused |

## Finding (task-332-03)
Capture with `-game`: void mean luma 0.16 against floor strips 18 to 49 (Q4 ok), no tiling, star twinkle visible, brightest star luma 108. The nebula (peak about 1.4 luma) and the dust are invisible at normal brightness, seen only with a 25x boost, and the dust is not distinguishable from the stars. To make them visible, raise Intensity on MI_Space_Nebula and MI_Space_Dust.

## Finding (task-333-01)
MI_Space_Nebula Intensity 0.06 to 1.6 (commit 975425cf) gave soft blurry violet blobs without structure; the owner judged it not working, task-335 replaced the nebula plane by generated texture layers ([[bp-space-texture-material]]). The 1.6 value stays on the unused instance.

- not_verified: parallax (camera static in the capture); nebula flow over time; packaged-build shader compile; exact instruction count; look at other arena edges.

## Relations
- [[blueprints-world-index]] — parent index.
- [[bp-space-background]] — actor that applies the three instances.
- [[bp-floor-plates-material]] — sibling material sharing the noise family.
- [[bp-space-texture-material]] — textured cloud and deep layers that replaced the nebula plane.
- [[mpp-player-light-mask]] — mask that dims the background far from the miner.
- [[task-332-space-background]] — global task with the decided values.
