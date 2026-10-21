---
id: lights-dark-ambient
description: "Provisional: dark violet ambient of L_ArenaM1 (sun, sky, fixed exposure, player light); values tuned per task, history and revert values inside."
parent: blueprints-world-index
kind: doc
---
# Dark violet ambient (L_ArenaM1)

Current look (task-275-01, owner 2026-10-21: back to Monday lighting, shader kept): sun 6 white with shadows, sky 0.3 white with shadows; exposure frozen at -0.94 EV100 (task-275-04); the darkening mask is kept, see [[mpp-player-light-mask]]. The history below is kept.

Provisional (owner 2026-10-20, task-265-01; owner tunes by eye). Reason: reference game look, a dark violet ambient with a warm pool of light around the player (the player light comes from task-263-02). The strong sun (intensity 6) burnt the yellow chasers white-yellow.

- asset_path: placed actors DirectionalLight_0 and SkyLight_0 of /Game/RogueRigs/Maps/L_ArenaM1 (external actor packages, same files as [[lights-no-shadows]]).

## Values (before -> after, read back)
Colours are set and read in the MCP float form (0 to 1 per channel, the editor stores them as 8-bit; read-back 0.749/0.620/1.0 and 0.349/0.251/0.600 is the 8-bit rounding of the values set).
- DirectionalLight_0 (LightComponent0): `intensity` 6 -> 1.0; `lightColor` (1, 1, 1) -> (0.75, 0.62, 1.0). Unchanged: `temperature` 6500 with `bUseTemperature` true (the temperature tint multiplies the colour; 6500 is near white), `indirectLightingIntensity` 1, `lightSourceAngle` 0.7357, `castShadows` false, `volumetricScatteringIntensity` 1.
- SkyLight_0 (SkyLightComponent0): `intensity` 0.3 -> 0.5; `lightColor` (1, 1, 1) -> (0.35, 0.25, 0.6). Unchanged: `indirectLightingIntensity` 1, `castShadows` false, `bRealTimeCapture` false, `sourceType` SLS_CapturedScene, no cubemap, `bLowerHemisphereIsBlack` true.

## Tight light (task-268-01, owner 2026-10-21, provisional, owner tunes by eye)
Owner played: still bright pink-violet everywhere, light too wide. Reference (DRGS): warm pool of about 0.18 of the screen width (about 400 uu at the camera) around the miner, the rest very dark violet. Values read before, set, read back:
- DirectionalLight_0 `intensity` 1.0 -> 0.12 (colour (0.75, 0.62, 1.0) and castShadows false kept). SkyLight_0 `intensity` 0.5 -> 0.08 (colour (0.35, 0.25, 0.6) and castShadows false kept).
- PlayerLight on BP_Miner ([[bp-miner]]): `attenuationRadius` 1400 -> 650, `intensity` 60 -> 120 cd. Colour, castShadows true and height 300 uu kept (a lower height was not judged better without seeing it). Reasoning: illuminance = cd / d^2 (d in metres), windowed to 0 at the radius. Old: 60 cd gave 15 at 2 m, 3.75 at 4 m, 1.3 at 6.5 m. New 120 cd: 30 at 2 m, 7.5 at 4 m, 0 at 6.5 m (about 2.8 at 5 m, windowed). So the core is twice as strong and the edge of the screen (about 10 m wide) is black. The floor pool radius is sqrt(650^2 - 300^2) = 577 uu at the 300 uu height; the visible bright pool is smaller (about 400 uu) because of the quadratic fall-off.
- Exposure ([[ppv-xp-glow]]): fixed. Method: `AutoExposureMinBrightness` = `AutoExposureMaxBrightness` = 1.3 (override boxes on; method stays AEM_Histogram; it is the simplest way to freeze eye adaptation, and AEM_Manual would instead read the camera values ISO 100, 1/60 s, f/4 = EV100 9.9, far too dark for lights this small). Unit: EV100 (the project enables `r.DefaultFeature.AutoExposure.ExtendDefaultLuminanceRange`, so the range defaults are -10 to 20 EV100). Previously min -10, max 20, not overridden (auto exposure). Choice of 1.3: estimate of the old auto exposure on the sun-lit arena (sun 1 lux + sky 0.5, albedo about 0.3, bias 1): about EV -0.7; +1 EV halves the brightness, so about EV 0.3 net; AutoExposureBias stays at its default 1, hence 1.3 as the clamp value. The estimate is not measured. Single number to tune: raise it to darken (each +1 halves the brightness), lower it to brighten.

## task-270-03 (owner 2026-10-21, provisional, tuned on -game captures judged by the Orchestrator)
Values picked from real -game captures (Saved/LookTune/r1, variant close to v6), each read before, set, read back:
- DirectionalLight_0 `intensity` 0.12 -> 1.5 (colour (0.75, 0.62, 1.0) and castShadows false kept).
- PlayerLight on BP_Miner ([[bp-miner]]): `attenuationRadius` 650 -> 520, `intensity` 120 -> 650 cd, `lightColor` (1.0, 0.62, 0.30) -> (1.0, 0.80, 0.59) (8-bit 255/205/150; read back 1.0 / 0.8039 / 0.5882). Height 300, castShadows true kept.
- SkyLight_0 (intensity 0.08, colour (0.35, 0.25, 0.6), castShadows false) and PPV_XPGlow (exposure min = max 1.3, bloom 1.3, threshold -1): unchanged, read back. Kamikaze CoreLights untouched.
- Reason: r0 captures showed the pool too wide and a dull orange, the scene beyond the pool pure black; the r1 sweep showed radius 520, about 700 cd, a cream colour and a sun of 1.2 to 2.0 matched the reference best.
- Revert (task-268 values): DirectionalLight_0 intensity 0.12; PlayerLight attenuationRadius 650, intensity 120, lightColor (1.0, 0.62, 0.30).

## task-270-05
- task-270-05 (round 3, owner 2026-10-21, provisional): PlayerLight relative Z 300 -> 450, `attenuationRadius` 520 -> 600, `intensity` 650 -> 2400 cd, `lightColor` (1.0, 0.804, 0.588) -> (1.0, 0.75, 0.50) (8-bit 255/191/128; read back 1.0 / 0.7490 / 0.5020); castShadows true kept; read back after compile and save. Reason: r2 captures showed chasers and rock tops near the miner burnt white because the light sits low; raising it keeps the floor pool reach about sqrt(600^2 - 450^2) = 397 uu and the centre illuminance about the same: 2400 cd / 4.5 m^2 x window 0.47 ~ 650 / 3 m^2 x 0.79. Revert = task-270-03 values (Z 300, radius 520, 650 cd, colour (1.0, 0.804, 0.588)).

## task-270-07
- task-270-07 (round 4, owner 2026-10-21, provisional): PlayerLight `attenuationRadius` 600 -> 680, `intensity` 2400 -> 3000 cd (read back 680 / 3000 after compile and save); height 450, colour 255/191/128 and castShadows kept. Reason: r3 captures showed the pool too small and dim (core L 99, half radius 150 px); a larger radius widens the floor reach to sqrt(680^2 - 450^2) = 510 uu and raises the window at the floor centre from 0.47 to 0.65. Revert = task-270-05 values (radius 600, 2400 cd).

## task-270-10
- task-270-10 (round 5, owner 2026-10-21, provisional): PlayerLight `attenuationRadius` 680 -> 640 (read back 640 after compile and save); intensity 3000 cd, height 450 and colour 255/191/128 kept. Reason: r4 captures pool too wide again (half radius 260 px, 0.27 of width; r3 at 600 gave 150 px); 640 aims at about 0.2. Revert = task-270-07 value 680.

## task-272
- task-272-02 (owner 2026-10-21): PlayerLight lowered to Z -15 (about 75 uu above the floor), radius 640 -> 3000, 3000 -> 400 cd, colour 255/194/158, for a smooth full-screen gradient with wall shadows; before/after and revert values in [[bp-miner]]. Sun and exposure: task-272-03 below.
- task-272-15 (owner 2026-10-21): the player light is removed from BP_Miner; the scene is lit flat by the shadowless sun and sky. The centre-brighter effect comes only from [[mpp-player-light-mask]]. Revert values: [[bp-miner]].
- task-272-18 (owner 2026-10-21, provisional): flat lighting: DirectionalLight_0 `intensity` 0.3 -> 4.0 (colour (0.75, 0.62, 1.0) and castShadows false kept, read back after save); SkyLight_0 (0.08) and exposure (min = max 1.8 EV100) unchanged. The centre-brighter effect is [[mpp-player-light-mask]]. Source: sweep task-272-16 (Saved/LookTune/r8, S10-S40): sun 4.0 reads evenly without the effect (floor about 25/255, rock tops 40-50, no hot spot); a directional light is kept so the floor normal map still shows relief. Revert: sun 0.3.
- task-272-03 (owner 2026-10-21, provisional): DirectionalLight_0 `intensity` 1.5 -> 0.3 (colour (0.75, 0.62, 1.0) and castShadows false kept, read back after save); PPV_XPGlow `AutoExposureMinBrightness` = `AutoExposureMaxBrightness` 1.3 -> 1.8 EV100 (overrides on, AutoExposureBias 1, method AEM_Histogram, bloom unchanged; +0.5 EV100 darkens by half a stop, equal to the runtime r.ExposureOffset -0.5 of the sweep). SkyLight_0 unchanged (0.08), read back. Reason: with the low player light the far floor and rock tops must fall toward the measured DRGS ring profile (far rings about 9 to 12 of 255); sweep task-272-05 variant M10 (sun 0.3, exposure -0.5) matched best (score 0.21 against 0.61 for the old values). Revert: sun 1.5, exposure min = max 1.3.

## task-274-03
- task-274-03 (owner 2026-10-21, provisional): PPV_XPGlow `AutoExposureMinBrightness` = `AutoExposureMaxBrightness` 1.8 -> -2.9 EV100 (read back -2.9; overrides on, method AEM_Histogram, bias 1 unchanged). Sun 4.0, sky 0.08, bloom 1.3 / -1 and the M_PP_PlayerLightMask blendable read back unchanged. Reason: the old mask multiplied the centre by 30 x WarmTint (luminance about x25 = +4.7 EV); the mask is now a pure darkener with gain 1 at the miner (task-274-02), so exposure raises the flat scene to the old centre brightness: 1.8 - 4.7 = -2.9. Exposure rather than sun, because emissives (XP, crystals, thrusters) were multiplied by the same old mask and keep their relation to the scene; the sun stays 4.0 for the floor normal-map relief. Value calibrated by the next sweep. Revert: exposure 1.8.

## task-274-05
- task-274-05 (owner 2026-10-21, provisional): PPV_XPGlow `AutoExposureMinBrightness` = `AutoExposureMaxBrightness` -2.9 -> -3.4 EV100 (read back -3.4; overrides on, bloom 1.3 / -1, blendable weight 1 unchanged). Reason: round-10 sweep variant Q4 (best match to the DRGS ring profile, A score 0.230), with the mask defaults of [[mpp-player-light-mask]]. Revert: -2.9, then 1.8.

## task-275-01
- task-275-01 (owner 2026-10-21): external packages of DirectionalLight_0 and SkyLight_0 restored from commit 75610b21 (Monday 2026-10-19). DirectionalLight_0: `intensity` 4.0 -> 6, `lightColor` (0.75, 0.62, 1.0) -> (1, 1, 1), `castShadows` false -> true. SkyLight_0: `intensity` 0.08 -> 0.3, `lightColor` (0.35, 0.25, 0.6) -> (1, 1, 1), `castShadows` false -> true. Editor read-back (task-275-04): DirectionalLight_0 intensity 6, lightColor (1, 1, 1), castShadows true; SkyLight_0 intensity 0.3, lightColor (1, 1, 1), castShadows true. Revert = task-272-18 state: sun 4.0 (0.75, 0.62, 1.0) no shadows, sky 0.08 (0.35, 0.25, 0.6) no shadows (git: checkout 1958a46f for the two packages).

## Notes and risks
- The sun has `bAtmosphereSunLight` true, but the level has no SkyAtmosphere actor, so the flag has no visible effect.
- The sky light captures the scene (SLS_CapturedScene, real-time capture off): it uses the capture baked in the editor, and its colour multiplies it. It contributes only what that capture holds; it was not recaptured here. If the ambient stays too black, recapture the sky light in the editor (Recapture) or give it a cubemap/specified colour. Not verified by eye.
- Auto exposure risk of task-265 is closed by the fixed exposure above (task-268-01).
- Bloom (PPV_XPGlow 1.3), Lumen and materials untouched.

## Revert
task-268-01 values: DirectionalLight_0 intensity 1.0, SkyLight_0 intensity 0.5, PlayerLight attenuationRadius 1400 and intensity 60, PPV_XPGlow: untick Auto Exposure Min and Max Brightness overrides (or set -10 and 20).
task-265 values:
Set DirectionalLight_0 intensity 6 and lightColor (1, 1, 1); SkyLight_0 intensity 0.3 and lightColor (1, 1, 1), by Details or `ObjectTools.set_properties` on `...L_ArenaM1:PersistentLevel.DirectionalLight_0.LightComponent0` and `...SkyLight_0.SkyLightComponent0`. Save as in the MCP save note of [[lights-no-shadows]].

- owner_agent: agent-bp

## task-275-04
- task-275-04 (owner 2026-10-21: fixed exposure at the Monday auto exposure): PPV_XPGlow `autoExposureMinBrightness` = `autoExposureMaxBrightness` -3.4 -> -0.94 EV100 (overrides on, AEM_Histogram, bias 1, bloom 1.3 / -1 and the mask blendable weight 1 unchanged). Measured in task-275-03: Monday PPV without mask, Monday lights, ShowFlag.VisualizeHDR in one -game run, settled Average Scene EV100 -0.945 / -0.943 / -0.943, smoothed pre-bias marker -0.93 / -0.94 / -0.94 (with-bias marker -1.94); bias multiplies after the min/max clamp, so min = max = -0.94 with bias 1 reproduces the adapted exposure. MPC_PlayerLight read back Enable 1, FarBrightness 0.03, Radius 250, Exponent 4 (no change). Revert: -3.4 in both fields (task-274-05 state).

## Relations
- [[blueprints-world-index]] — parent index.
- [[lights-no-shadows]] — same two actors, shadow flags and the MCP save method.
- [[ppv-xp-glow]] — bloom, and the fixed exposure (task-268-01).
- [[bp-miner]] — the PlayerLight whose radius and intensity changed (removed in task-272-15, revert values there).
- [[mpp-player-light-mask]] — the post-process mask that gives the centre-brighter effect since task-272-15.
- [[visual-style]] — the look statement.
