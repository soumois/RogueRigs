---
id: mpp-player-light-mask
description: "Owner guide to the player light mask: MPC_PlayerLight parameters (Enable, CenterBrightness, FarBrightness, Radius, Exponent, tints), how to tune, disable and remove it."
parent: blueprints-world-index
kind: doc
---
# MPC_PlayerLight and M_PP_PlayerLightMask (player light mask)

- asset_path: /Game/RogueRigs/Materials/PostProcess/MPC_PlayerLight (Material Parameter Collection) and /Game/RogueRigs/Materials/PostProcess/M_PP_PlayerLightMask (Material, domain Post Process, blendable in PPV_XPGlow). No Blueprint, no C++ class, no graph logic.
- what_it_is: owner direction 2026-10-21: lighting is flat and shadowless, with no player point light. The only "centre brighter" effect is this screen-space mask: every pixel is multiplied by a brightness that is high near the miner and low far away, with a warm tint near and a dark magenta tint far. Only C++ writes PlayerPosition (the miner LightMask component, see [[sys-player]]); everything else is yours.
- where_to_edit: Content Browser > RogueRigs/Materials/PostProcess/MPC_PlayerLight, change the default values. Edits apply live in PIE and in game.
- disable: Enable = 0 (output = scene colour). Remove fully: delete the M_PP_PlayerLightMask entry in the blendables of PPV_XPGlow ([[ppv-xp-glow]]).

| name | default | neutral default / revert | unit | meaning and tuning |
|---|---|---|---|---|
| Enable | 1 | 0 | 0 or 1 | 0 = effect off, 1 = on. Set to 1 by task-272-17. |
| CenterBrightness | 30 | 3.0 | multiplier | brightness at the miner. Raise for a hotter centre. |
| FarBrightness | 0.15 | 0.4 | multiplier | floor of the mask far from the miner. Raise if the edges are too dark. |
| Radius | 200 | 225 | uu | distance where the extra brightness has halved (about 200 px on screen). Raise to widen. |
| Exponent | 3 | 2 | none | falloff shape: 2 = long inverse-square-like tail, higher = tighter. |
| TintStrength | 1 | 1 | 0 to 1 | 0 = no colour change, 1 = full tint. |
| WarmTint | (1.0, 0.9, 0.06) | (1.0, 0.76, 0.62) | colour | colour near the miner. |
| FarTint | (1.0, 0.7, 0.55) | (0.85, 0.55, 0.9) | colour | colour far away (dark magenta side). |
| PlayerPosition | (0,0,0) | (0,0,0) | world uu | written by C++ each tick, XY used. Do not edit. |

- formula (Custom HLSL): r = length(WorldPos.xy - PlayerPosition.xy); mask = 1 / (1 + pow(r / max(Radius, 1), Exponent)); gain = FarBrightness + (CenterBrightness - FarBrightness) * mask; tint = lerp(1, lerp(FarTint, WarmTint, mask), TintStrength); out = lerp(SceneColor, SceneColor * gain * tint, saturate(Enable)). WorldPos is the engine WorldPosition node (scene world position of each pixel). Blendable location Scene Color Before Bloom (HDR).
- tuning notes (task-272-17, sweep variant Q9, best match to the DRGS ring profile with the flat sun at 4.0): CenterBrightness is large because the flat scene sits around 25/255. CenterBrightness and FarBrightness scale together with exposure: if exposure is raised, divide both by the same factor. WarmTint has almost no blue because the violet sun makes the centre pink otherwise; if the miner looks too yellow, try WarmTint (1, 0.8, 0.12).
- console: the C++ writer overwrites the parameters above only when bDriveTunables is set from the console, with exactly these names (sys-player).
- history: task-272-06 first mask (MaskRadius, NearGain, FarGain, AddIntensity, NearColour, FarColour). task-272-12 centre fix: hand-made world position replaced by the WorldPosition node. task-272-14 parameters rebuilt around the owner names above; old ones removed. task-272-17 defaults set from the task-272-16 sweep, effect enabled (Enable 1).
- not_verified: centring and look at runtime; instruction count.
- owner_agent: agent-bp

## Relations
- [[blueprints-world-index]] — parent index.
- [[ppv-xp-glow]] — the volume that carries the material as a blendable.
- [[lights-dark-ambient]] — the flat dark ambient this mask brightens around the miner.
- [[sys-player]] — the miner LightMask component that writes PlayerPosition.
