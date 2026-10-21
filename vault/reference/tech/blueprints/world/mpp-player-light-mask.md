---
id: mpp-player-light-mask
description: "Owner guide to the player light mask darkener: MPC_PlayerLight parameters (Enable, FarBrightness, Radius, Exponent), formula, how to tune, disable and remove it, removed parameters."
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
| FarBrightness | 0.03 | multiplier (0 to 1) | brightness far from the miner (1 = no darkening). Previous 0.002 (task-274-08), before that 0.005 (task-274-02). |
| Radius | 250 | uu | distance where the gain is halfway between FarBrightness and 1. Raise to widen the lit zone. Previous 260 (task-274-08), before that 200. |
| Exponent | 4 | none | falloff shape: 2 = long tail, higher = tighter edge. Previous 3. |
| PlayerPosition | (0,0,0) | world uu | written by C++ each tick, XY used. Do not edit. |

- formula (Custom HLSL, Scene Color Before Bloom): r = length(WorldPos.xy - PlayerPosition.xy); mask = 1 / (1 + pow(r / max(Radius, 1), Exponent)); gain = lerp(FarBrightness, 1, mask); out = lerp(SceneColor, SceneColor * gain, saturate(Enable)). WorldPos is the engine WorldPosition node.
- console: the C++ writer overwrites the parameters above only when bDriveTunables is set from the console, with exactly these names (sys-player).

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
- history: task-272-06 first mask. task-272-12 WorldPosition node. task-272-14 parameters renamed. task-272-17 defaults from the sweep, Enable 1. task-274-02 darkener: CenterBrightness, TintStrength, WarmTint, FarTint removed, FarBrightness 0.15 to 0.005.
- not_verified: look at runtime; instruction count; FarBrightness calibration.
- owner_agent: agent-bp

## Relations
- [[blueprints-world-index]] — parent index.
- [[ppv-xp-glow]] — the volume that carries the material as a blendable.
- [[lights-dark-ambient]] — the flat dark ambient this mask darkens away from the miner.
- [[sys-player]] — the miner LightMask component that writes PlayerPosition.
