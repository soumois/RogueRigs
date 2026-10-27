---
id: bp-space-texture-material
description: "M_SpaceTexture and instances MI_Space_Deep, MI_Space_Clouds: unlit additive generated-texture space layers, parameters, final intensities, capture finding, revert; open to tune them."
parent: blueprints-world-index
kind: doc
---
# M_SpaceTexture

- asset_path: /Game/RogueRigs/Environment/Space/M_SpaceTexture; instances MI_Space_Deep and MI_Space_Clouds, textures T_Space_Deep and T_Space_Clouds, same folder (task-335-03, commit 31991c2b, provisional).
- parent_class: UMaterial (instances: MaterialInstanceConstant of the master)
- purpose: draws a tileable AI-generated space texture on a plane of [[bp-space-background]]: deep colour washes (DeepPlane) and cloud filaments (NebulaPlane).
- components: not applicable (material).
- exposed_defaults: see the parameter table below.
- events: none.
- logic_in_bp: none. Unlit, Additive, TwoSided, no shadow. Emissive = texture x Tint x Intensity, with Contrast and Saturation parameters. UV = world XY / TileSize + Time x PanVelocity (slow drift, no rotation). Textures sRGB, mips, Wrap.
- assets: T_Space_Deep, T_Space_Clouds (2048, tileable, made by `make_tileable.js`; prompts and method in [[art-prompts-space-background]]).
- owner_agent: agent-bp
- look_rule: the void far gain of the mask is 0.5 ([[mpp-player-light-mask]]); target cloud peak 70 to 100 sRGB luma (B1 of [[task-335-space-texture-layers]]).
- revert: set NebulaPlane back to MI_Space_Nebula and delete DeepPlane in [[bp-space-background]]; the master and instances then have no user. M_SpaceBackground is untouched.

## Instances
| Parameter | MI_Space_Deep | MI_Space_Clouds |
|---|---|---|
| Intensity (final, task-335-07, b7fbf824) | 0.7 | 0.6 (was 2.7 at task-335-03) |
| Contrast | 1.3 | 1.6 |
| TileSize (uu) | about 20000 | about 12000 |
| Drift (uu/s) | about 5 | about 12, other direction |
| Plane | DeepPlane Z -8000 | NebulaPlane Z -5000 |

Exact Tint, Saturation, TileSize and PanVelocity read-back values: open (not in the change summaries; read them in the instances).

## Finding
- task-335-06 (before retune, Intensity 2.7): cloud p99 180 / max 232, void mean 45.5 above floor strips 34.6 to 37.3. Layers dense, no tiling, no plane edge, drift about 2 px per 3 s; deep layer not distinguishable.
- task-335-07: Intensity 2.7 to 0.6, computed ratio 0.222.
- task-335-08 (Intensity 0.6): cloud p99 116 (band 70 to 100 missed), void mean 20.5 below floor strips 33.7 to 39.0 (pass), deep layer still not distinguishable, near-black share of the void 19.2 percent. Tonemapping made the change x0.64, not x0.22.
- decision: the owner accepted the cloud peak 116 as a deviation kept on purpose ("Ok pour ça, passe à la 334", 2026-10-27); no further retune.
- not_verified: deep layer visibility; packaged-build shader compile; other arena edges.

## Relations
- [[blueprints-world-index]] — parent index.
- [[bp-space-background]] — actor whose DeepPlane and NebulaPlane use these instances.
- [[bp-space-background-material]] — procedural master (stars, dust) kept beside it.
- [[mpp-player-light-mask]] — void far gain 0.5 applied to these layers.
- [[art-prompts-space-background]] — prompts and method of the two textures.
- [[task-335-space-texture-layers]] — global task with the decided values.
