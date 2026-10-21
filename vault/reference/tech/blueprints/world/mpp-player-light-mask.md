---
id: mpp-player-light-mask
description: "MPC_PlayerLight and M_PP_PlayerLightMask: radial post-process light mask around the player (DRGS-style brightening), neutral by default; open to tune or wire it."
parent: blueprints-world-index
kind: doc
---
# MPC_PlayerLight and M_PP_PlayerLightMask (player light mask)

- asset_path: /Game/RogueRigs/Materials/PostProcess/MPC_PlayerLight (Material Parameter Collection) and /Game/RogueRigs/Materials/PostProcess/M_PP_PlayerLightMask (Material, domain Post Process). Task-272-06, provisional, owner 2026-10-21.
- parent_class: MaterialParameterCollection and Material (no Blueprint, no C++ class).
- purpose: owner 2026-10-21, task-272: DRGS shows a radial brightening of everything near the player, not a directional light. A faint shadow-casting PlayerLight keeps only the wall shadows; this mask does the brightening, uniform on every pixel whatever its normal (floor, rocks, robots, pickups).
- components: one Custom HLSL node (output Float3 to Emissive Color) fed by SceneTexture PostProcessInput0 (colour), SceneTexture SceneDepth and nine Collection Parameter nodes. Blend mode Opaque.
- exposed_defaults (MPC, read back): scalars MaskRadius 220 (uu, R0), MaskExponent 2, NearGain 0, FarGain 1, AddIntensity 0, MaskEnabled 0; vectors PlayerPosition (0,0,0,0, world position, XY used), NearColour (1, 0.76, 0.62, 1), FarColour (1, 1, 1, 1).
- formula: world position rebuilt from SvPosition and SceneDepth (a post-process WorldPosition node gives only the screen plane). r = XY distance to PlayerPosition (same mask at every height). mask = 1 / (1 + (r / max(MaskRadius, 1))^MaskExponent). tint = lerp(FarColour, NearColour, mask). lit = SceneColor * (FarGain + NearGain * mask) * tint + AddIntensity * mask * NearColour. Output = lerp(SceneColor, lit, saturate(MaskEnabled)).
- events: the C++ writer (task-272-07) pushes PlayerPosition and the tunables into the collection each tick. Nothing else reads it.
- logic_in_bp: none. Neutral by default: MaskEnabled 0 gives output = scene colour (also NearGain 0, FarGain 1, white FarColour, AddIntensity 0).
- assets: blendable location is Scene Color Before Bloom (HDR, before tonemapping). The editor reset Before Tonemapping to After DOF when set through MCP, so Before Bloom was used (task-272-06). The material is in the PPV_XPGlow blendables since task-272-08 (weight 1.0, neutral while MaskEnabled is 0, see [[ppv-xp-glow]]). Shader instruction count not read (no MCP tool).
- owner_agent: agent-bp

## Relations
- [[blueprints-world-index]] — parent index.
- [[ppv-xp-glow]] — the volume that carries the material as a blendable (task-272-08).
- [[lights-dark-ambient]] — the dark ambient this mask brightens around the miner.
- [[sys-player]] — the miner LightMask component (URRPlayerLightMaskComponent) that writes the collection each tick (task-272-07).
