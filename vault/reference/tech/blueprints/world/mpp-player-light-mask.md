---
id: mpp-player-light-mask
description: "Guide to the screen radial darkener: MPC_PlayerLight parameters (Enable, CenterDarkness, CornerDarkness, ClearRadius, FadeRadius, VoidMaxDarkness, VoidZ), formula, tuning, disable, reverts, verification state."
parent: blueprints-world-index
kind: doc
---
# MPC_PlayerLight and M_PP_PlayerLightMask (screen radial darkener)

- asset_path: /Game/RogueRigs/Materials/PostProcess/MPC_PlayerLight (Material Parameter Collection) and /Game/RogueRigs/Materials/PostProcess/M_PP_PlayerLightMask (Material, domain Post Process, blendable in PPV_XPGlow). No Blueprint, no C++ class, no graph logic.
- what_it_is: since task-426-01 (41c3dca9, owner 2026-11-04, [[task-426-screen-radial-darkener]]) a pure screen-space darkener, fixed on screen: black added = lerp(CenterDarkness, CornerDarkness, t), t = 0 at the screen centre and 1 at the corners, circles in pixels, linear fade, multiplied since task-427-01 (cb27e12f, [[task-427-darkener-clear-zone]]) by a clear zone around the centre. It no longer depends on the miner or the world, except the void exception. No tint, no brightening.
- where_to_edit: Content Browser > RogueRigs/Materials/PostProcess/MPC_PlayerLight, change the default values. Edits apply live in PIE and in game.
- disable: Enable = 0 (output = scene colour). Remove fully: delete the M_PP_PlayerLightMask entry in the blendables of PPV_XPGlow ([[ppv-xp-glow]]; its entry is unchanged, same asset name).

| name | default | unit | meaning and tuning |
|---|---|---|---|
| Enable | 1 | 0 or 1 | 0 = effect off, 1 = on. |
| CenterDarkness | 0.2 | 0 to 1 | black added at the screen centre (0 = none). |
| CornerDarkness | 1 | 0 to 1 | black added at the corners (1 = fully black; owner tuned 0.85 to 1). |
| ClearRadius | 0.15 | share of centre-to-corner distance (t) | no black for t up to this value. Raise for a bigger clear centre. |
| FadeRadius | 0.35 | same (t) | smoothstep from no black at ClearRadius to the full lerp at FadeRadius. If FadeRadius <= ClearRadius: hard edge at ClearRadius. |
| VoidMaxDarkness | 0.5 | 0 to 1 | cap of the black on void pixels (world Z below VoidZ: stars plane, texture layers, dust). |
| VoidZ | -200 | world uu | pixels with WorldPos.z below this are void. |
| PlayerPosition | (0,0,0,0) | world uu | vector kept, still written by C++ ([[sys-player]]) but no longer read by the shader (Q10: C++ left as is). |

- formula (Custom HLSL, Scene Color Before Bloom; inputs SceneCol, WorldPos, UV = ScreenPosition ViewportUV, ViewSz = ViewSize, CenterD, CornerD, ClearR, FadeR, VoidMax, Enable, VoidZ): d = (UV - 0.5) * ViewSz; t = saturate(length(d) / max(0.5 * length(ViewSz), 1)); black = lerp(CenterD, CornerD, t); m = (FadeR > ClearR) ? smoothstep(ClearR, FadeR, t) : (t >= ClearR ? 1 : 0); black = black * m; if (WorldPos.z < VoidZ) black = min(black, VoidMax); out = SceneCol.rgb * (1 - black * saturate(Enable)).
- removed parameters: FarBrightness, VoidFarBrightness, Radius, Exponent.
- verification task-427-02 (one before/after pair, Saved/Captures/task427-02/ off_00.png, on_00.png, contact_sheet.png, 1920x1080, Enable toggled at runtime): C1 seen (after/before ratio about 1.0 within t 0.15); C2 seen (ratio falls 0.99 to 0.55 over t 0.15 to 0.35, no ring); C3 seen (circles, darkest at corners); C4 ambiguous, left to the owner: corner after/before 0.004 to 0.15, mean about 0.08 with CornerDarkness 1 (about 0.4 expected for 0.85); the bottom-right corner was already near black before. Not run: tonemapper-off pair, light on/off checks (Q16, Q17), moved-miner frame (seen in task-426).
- verification (task-426-02, frames in Saved/Captures/task426-02b/): circles, linear fade and fixed-on-screen seen. Measured corner colour about 0.27 to 0.60 of the undarkened frame instead of 0.15; likely display-space after tonemap, unresolved, owner call. Void exception not verified in render.

## Revert to the task-426 screen darkener (no clear zone)
Remove the two lines `m = ...` and `black = black * m`, remove the ClearR and FadeR CollectionParameter inputs and the ClearRadius and FadeRadius scalars. Or `git revert cb27e12f`.

## Revert to the old world-space mask
Restore the HLSL: r = length(WorldPos.xy - PlayerPos.xy); mask = 1 / (1 + pow(r / max(Radius, 1), Exponent)); farB = WorldPos.z < VoidZ ? VoidFar : FarBrightness; gain = lerp(farB, 1, mask); out = lerp(SceneCol.rgb, SceneCol.rgb * gain, saturate(Enable)). Restore MPC defaults: Enable 1, FarBrightness 0.06, Radius 200, Exponent 1.8, VoidFarBrightness 0.5, VoidZ -200, and delete CenterDarkness, CornerDarkness, VoidMaxDarkness. Or `git revert 41c3dca9`.

- history: task-272-06 first mask; task-274-02 darkener (CenterBrightness, TintStrength, WarmTint, FarTint removed); task-274-05, 274-08, 276 tuned FarBrightness, Radius, Exponent; task-335-05 void gain (VoidFarBrightness, VoidZ); task-426-01 screen radial; task-427-01 clear zone. Older values are in git history.
- owner_agent: agent-bp

## Relations
- [[blueprints-world-index]] — parent index.
- [[ppv-xp-glow]] — the volume that carries the material as a blendable.
- [[lights-dark-ambient]] — the flat dark ambient this mask darkens.
- [[bp-space-texture-material]] — textured void layers that the void cap lights.
- [[sys-player]] — the miner LightMask component that still writes PlayerPosition.
- [[task-426-screen-radial-darkener]] — the task that reworked it.
- [[task-427-darkener-clear-zone]] — the task that added ClearRadius and FadeRadius.
