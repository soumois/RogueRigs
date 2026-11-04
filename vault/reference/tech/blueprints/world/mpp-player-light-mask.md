---
id: mpp-player-light-mask
description: "Guide to the screen radial darkener: MPC_PlayerLight parameters (Enable, CenterDarkness, CornerDarkness, VoidMaxDarkness, VoidZ), formula, tuning, disable, revert to the old world-space mask, verification state."
parent: blueprints-world-index
kind: doc
---
# MPC_PlayerLight and M_PP_PlayerLightMask (screen radial darkener)

- asset_path: /Game/RogueRigs/Materials/PostProcess/MPC_PlayerLight (Material Parameter Collection) and /Game/RogueRigs/Materials/PostProcess/M_PP_PlayerLightMask (Material, domain Post Process, blendable in PPV_XPGlow). No Blueprint, no C++ class, no graph logic.
- what_it_is: since task-426-01 (41c3dca9, owner 2026-11-04, [[task-426-screen-radial-darkener]]) a pure screen-space darkener, fixed on screen: black added = lerp(CenterDarkness, CornerDarkness, t), t = 0 at the screen centre and 1 at the corners, circles in pixels, linear fade. It no longer depends on the miner or the world, except the void exception. No tint, no brightening.
- where_to_edit: Content Browser > RogueRigs/Materials/PostProcess/MPC_PlayerLight, change the default values. Edits apply live in PIE and in game.
- disable: Enable = 0 (output = scene colour). Remove fully: delete the M_PP_PlayerLightMask entry in the blendables of PPV_XPGlow ([[ppv-xp-glow]]; its entry is unchanged, same asset name).

| name | default | unit | meaning and tuning |
|---|---|---|---|
| Enable | 1 | 0 or 1 | 0 = effect off, 1 = on. |
| CenterDarkness | 0 | 0 to 1 | black added at the screen centre (0 = none). |
| CornerDarkness | 0.85 | 0 to 1 | black added at the corners (0.85 leaves 15 % of the colour). |
| VoidMaxDarkness | 0.5 | 0 to 1 | cap of the black on void pixels (world Z below VoidZ: stars plane, texture layers, dust). |
| VoidZ | -200 | world uu | pixels with WorldPos.z below this are void. |
| PlayerPosition | (0,0,0,0) | world uu | vector kept, still written by C++ ([[sys-player]]) but no longer read by the shader (Q10: C++ left as is). |

- formula (Custom HLSL, Scene Color Before Bloom; inputs SceneCol, WorldPos, UV = ScreenPosition ViewportUV, ViewSz = ViewSize, CenterD, CornerD, VoidMax, Enable, VoidZ): d = (UV - 0.5) * ViewSz; t = saturate(length(d) / max(0.5 * length(ViewSz), 1)); black = lerp(CenterD, CornerD, t); if (WorldPos.z < VoidZ) black = min(black, VoidMax); out = SceneCol.rgb * (1 - black * saturate(Enable)).
- removed parameters: FarBrightness, VoidFarBrightness, Radius, Exponent.
- verification (task-426-02, frames in Saved/Captures/task426-02b/): circles, linear fade and fixed-on-screen seen. Measured corner colour about 0.27 to 0.60 of the undarkened frame instead of 0.15; likely display-space after tonemap, unresolved, owner call. Void exception not verified in render.

## Revert to the old world-space mask
Restore the HLSL: r = length(WorldPos.xy - PlayerPos.xy); mask = 1 / (1 + pow(r / max(Radius, 1), Exponent)); farB = WorldPos.z < VoidZ ? VoidFar : FarBrightness; gain = lerp(farB, 1, mask); out = lerp(SceneCol.rgb, SceneCol.rgb * gain, saturate(Enable)). Restore MPC defaults: Enable 1, FarBrightness 0.06, Radius 200, Exponent 1.8, VoidFarBrightness 0.5, VoidZ -200, and delete CenterDarkness, CornerDarkness, VoidMaxDarkness. Or `git revert 41c3dca9`.

- history: task-272-06 first mask; task-274-02 darkener (CenterBrightness, TintStrength, WarmTint, FarTint removed); task-274-05, 274-08, 276 tuned FarBrightness, Radius, Exponent; task-335-05 void gain (VoidFarBrightness, VoidZ); task-426-01 screen radial. Older values are in git history.
- owner_agent: agent-bp

## Relations
- [[blueprints-world-index]] — parent index.
- [[ppv-xp-glow]] — the volume that carries the material as a blendable.
- [[lights-dark-ambient]] — the flat dark ambient this mask darkens.
- [[bp-space-texture-material]] — textured void layers that the void cap lights.
- [[sys-player]] — the miner LightMask component that still writes PlayerPosition.
- [[task-426-screen-radial-darkener]] — the task that reworked it.
