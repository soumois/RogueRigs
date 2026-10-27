---
id: mpp-player-light-mask
description: "Guide to the player light mask darkener: MPC_PlayerLight parameters (Enable, FarBrightness, VoidFarBrightness, VoidZ, Radius, Exponent), formula, how to tune, disable and remove it, removed parameters."
parent: blueprints-world-index
kind: doc
---
# MPC_PlayerLight and M_PP_PlayerLightMask (player light mask, darkener)

- asset_path: /Game/RogueRigs/Materials/PostProcess/MPC_PlayerLight (Material Parameter Collection) and /Game/RogueRigs/Materials/PostProcess/M_PP_PlayerLightMask (Material, domain Post Process, blendable in PPV_XPGlow). No Blueprint, no C++ class, no graph logic.
- what_it_is: owner direction 2026-10-21: lighting is flat and shadowless, with no player point light. This screen-space mask only darkens: every pixel is multiplied by a gain between FarBrightness (far from the miner) and 1 (at the miner). No tint, no gain above 1. Only C++ writes PlayerPosition (the miner LightMask component, see [[sys-player]]); everything else is yours.
- where_to_edit: Content Browser > RogueRigs/Materials/PostProcess/MPC_PlayerLight, change the default values. Edits apply live in PIE and in game.
- disable: Enable = 0 (output = scene colour). Remove fully: delete the M_PP_PlayerLightMask entry in the blendables of PPV_XPGlow ([[ppv-xp-glow]]).

| name | default | unit | meaning and tuning |
|---|---|---|---|
| Enable | 1 | 0 or 1 | 0 = effect off, 1 = on. |
| FarBrightness | 0.06 (saved value; documented 0.08, mismatch found in task-335-05 and 06, not explained) | multiplier (0 to 1) | brightness far from the miner (1 = no darkening). Previous 0.03 (owner edit 2026-10-21, task-276), before that 0.002 (task-274-08), before that 0.005 (task-274-02). |
| VoidFarBrightness | 0.5 | multiplier (0 to 1) | far gain of void pixels (world Z below VoidZ: stars plane, texture layers, dust). Added in task-335-05 (066b72d1, B2 of [[task-335-space-texture-layers]]). Revert: delete it and use FarBrightness in the formula (void then about 0.06 to 0.08). |
| VoidZ | -200 | world uu | pixels with WorldPos.z below this use VoidFarBrightness. Added in task-335-05. |
| Radius | 200 | uu | distance where the gain is halfway between FarBrightness and 1. Raise to widen the lit zone. Previous 250 (owner edit 2026-10-21, task-276), before that 260 (task-274-08), before that 200. |
| Exponent | 1.8 | none | falloff shape: 2 = long tail, higher = tighter edge. Previous 4 (owner edit 2026-10-21, task-276), before that 3. |
| PlayerPosition | (0,0,0) | world uu | written by C++ each tick, XY used. Do not edit. |

- formula (Custom HLSL, Scene Color Before Bloom): r = length(WorldPos.xy - PlayerPosition.xy); mask = 1 / (1 + pow(r / max(Radius, 1), Exponent)); farB = WorldPos.z < VoidZ ? VoidFarBrightness : FarBrightness; gain = lerp(farB, 1, mask); out = lerp(SceneColor, SceneColor * gain, saturate(Enable)). WorldPos is the engine WorldPosition node.
- console: the C++ writer overwrites the parameters above only when bDriveTunables is set from the console, with exactly these names (sys-player).

- task-335-05 (066b72d1): formula before: gain = lerp(FarBrightness, 1, mask). Floor and cliff faces checked from code only, render unverified; cliff faces below Z -200, if any, would take the void gain (not checked).

## Removed in task-274-02 (values to revert)
| name | type | previous default | previous neutral / revert |
|---|---|---|---|
| CenterBrightness | scalar, multiplier | 30 | 3.0 |
| TintStrength | scalar, 0 to 1 | 1 | 1 |
| WarmTint | vector, colour | (1.0, 0.9, 0.06) | (1.0, 0.76, 0.62) |
| FarTint | vector, colour | (1.0, 0.7, 0.55) | (0.85, 0.55, 0.9) |

Previous FarBrightness was 0.15 (neutral 0.4) and previous formula: gain = FarBrightness + (CenterBrightness - FarBrightness) * mask; tint = lerp(1, lerp(FarTint, WarmTint, mask), TintStrength); out = lerp(SceneColor, SceneColor * gain * tint, saturate(Enable)).

- task-274-05 (owner 2026-10-21, provisional): defaults FarBrightness 0.005 -> 0.002, Radius 200 -> 260, Exponent 3 -> 4 (read back; Enable 1 kept). Reason: round-10 sweep (variant Q4) best match to the DRGS ring profile, A score 0.230. Paired with exposure -3.4 in [[ppv-xp-glow]]. Revert: 0.005 / 200 / 3.
- task-274-08 (owner 2026-10-21, provisional): defaults FarBrightness 0.002 -> 0.03, Radius 260 -> 250, Exponent 4 kept (read back; Enable 1 kept). Reason: round-12 floor-only sweep: far floor 9.6-10.2/255 and 350 uu about 39/255; brighter far rock tops about 75/255 accepted by the owner. Revert: 0.002 / 260 / 4.
- task-276 (owner edit in the editor, 2026-10-21, saved and committed by the owner as 15dc423d): defaults FarBrightness 0.03 -> 0.08, Radius 250 -> 200, Exponent 4 -> 1.8, Enable 1 kept. Confirmed from the saved MPC_PlayerLight.uasset (float values in the package before and after); not read in the editor. Reason: owner tuning by eye, paired with the sun 6 -> 3 lux of [[lights-dark-ambient]]. Revert: 0.03 / 250 / 4.
- history: task-272-06 first mask. task-272-12 WorldPosition node. task-272-14 parameters renamed. task-272-17 defaults from the sweep, Enable 1. task-274-02 darkener: CenterBrightness, TintStrength, WarmTint, FarTint removed, FarBrightness 0.15 to 0.005.
- not_verified: look at runtime; instruction count; FarBrightness calibration.
- owner_agent: agent-bp

## Relations
- [[blueprints-world-index]] — parent index.
- [[ppv-xp-glow]] — the volume that carries the material as a blendable.
- [[lights-dark-ambient]] — the flat dark ambient this mask darkens away from the miner.
- [[bp-space-texture-material]] — textured void layers that this void gain lights.
- [[sys-player]] — the miner LightMask component that writes PlayerPosition.
