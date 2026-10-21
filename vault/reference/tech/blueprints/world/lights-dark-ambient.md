---
id: lights-dark-ambient
description: "Provisional: dark violet ambient of L_ArenaM1 (sun 0.12, sky 0.08, fixed exposure, tight player light); previous values for revert."
parent: blueprints-world-index
kind: doc
---
# Dark violet ambient (L_ArenaM1)

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

## Relations
- [[blueprints-world-index]] — parent index.
- [[lights-no-shadows]] — same two actors, shadow flags and the MCP save method.
- [[ppv-xp-glow]] — bloom, and the fixed exposure (task-268-01).
- [[bp-miner]] — the PlayerLight whose radius and intensity changed.
- [[visual-style]] — the look statement.
