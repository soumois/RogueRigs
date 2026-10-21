---
id: lights-dark-ambient
description: "Provisional: dark violet ambient of L_ArenaM1 (sun 1.0 violet, sky light 0.5 dark violet); previous values for revert. Open to tune or revert."
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

## Notes and risks
- The sun has `bAtmosphereSunLight` true, but the level has no SkyAtmosphere actor, so the flag has no visible effect.
- The sky light captures the scene (SLS_CapturedScene, real-time capture off): it uses the capture baked in the editor, and its colour multiplies it. It contributes only what that capture holds; it was not recaptured here. If the ambient stays too black, recapture the sky light in the editor (Recapture) or give it a cubemap/specified colour. Not verified by eye.
- Auto exposure risk: no exposure override on the PostProcessVolume ([[ppv-xp-glow]]) and none found in the project config, so the engine default eye adaptation applies and may brighten a dark scene back. Not changed.
- Bloom (PPV_XPGlow 1.3), Lumen and materials untouched.

## Revert
Set DirectionalLight_0 intensity 6 and lightColor (1, 1, 1); SkyLight_0 intensity 0.3 and lightColor (1, 1, 1), by Details or `ObjectTools.set_properties` on `...L_ArenaM1:PersistentLevel.DirectionalLight_0.LightComponent0` and `...SkyLight_0.SkyLightComponent0`. Save as in the MCP save note of [[lights-no-shadows]].

- owner_agent: agent-bp

## Relations
- [[blueprints-world-index]] — parent index.
- [[lights-no-shadows]] — same two actors, shadow flags and the MCP save method.
- [[ppv-xp-glow]] — bloom and exposure, untouched.
- [[visual-style]] — the look statement.
