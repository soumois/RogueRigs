---
id: lights-no-shadows
description: "Provisional: shadow casting off on the sun and sky light of L_ArenaM1; previous flags and revert steps. Open to bring shadows back."
parent: blueprints-world-index
kind: doc
---
# Lights without shadows (L_ArenaM1)

Provisional, temporary (owner 2026-10-20, task-261: "aucun objet ne doit avoir une ombre pour l'instant"). Method: shadow casting is off on the lights, never on the objects, so the per-object CastShadow flags are untouched and the change reverts from the lights alone.

- asset_path: placed actors in the persistent level /Game/RogueRigs/Maps/L_ArenaM1 (classes DirectionalLight and SkyLight, not Blueprints; external actor packages under Content/__ExternalActors__/RogueRigs/Maps/L_ArenaM1/). L_ArenaM1 is the only level of the project (Content/RogueRigs/Maps holds nothing else) and the only one the run loads.

## Lights changed
- DirectionalLight_0 (component LightComponent0, Movable, intensity 6): `castShadows` true -> false. Unchanged: `castStaticShadows` true, `castDynamicShadows` true (both only apply when `castShadows` is on), `bCastVolumetricShadow` true, contact shadow length 0, `castRaytracedShadow` UseProjectSetting, `bUseRayTracedDistanceFieldShadows` false, dynamic shadow distance 20000.
- SkyLight_0 (component SkyLightComponent0, Movable, intensity 0.3, real-time capture off): `castShadows` true -> false. Unchanged: `castStaticShadows` true, `castDynamicShadows` true, `bCastVolumetricShadow` true, `castRaytracedShadow` UseProjectSetting.

## Lights checked and left
- BP_Kamikaze, component CoreLight (point light, Movable): `castShadows` was already false, nothing changed.
- No other light in the level or in a Blueprint of the run (PointLight, SpotLight, RectLight search in Content/RogueRigs). The cheat light of `rr.Cheat.LightHalo` ([[sys-cheats]]) is created at runtime with shadows off by default.

## Not changed (separate sources of dark shapes, reported only)
- Lumen global illumination and reflections (project setting `r.DynamicGlobalIlluminationMethod=1`) and Virtual Shadow Maps (`r.Shadow.Virtual.Enable=1`): kept as they are. Lumen indirect occlusion can still darken contact areas.
- Capsule shadows: BP_Miner mesh `bCastCapsuleDirectShadow` false and `bCastCapsuleIndirectShadow` false, BP_Chaser same: no capsule shadow.
- Per-object CastShadow flags (meshes, ArenaGrid components) are on or off as before and are not part of this change.
- Decals: only the ground telegraph and explosion decals, which are gameplay marks, not shadows. No ShadowProxy mesh, no blob shadow actor found.

## Revert (bring the shadows back)
1. Open L_ArenaM1, select DirectionalLight_0, Details, Light, Cast Shadows: tick it.
2. Select SkyLight_0, Details, Light, Cast Shadows: tick it.
3. Save the level (Ctrl+S) so the external actor files are written. Or by MCP: `ObjectTools.set_properties` with `{"castShadows": true}` on `...L_ArenaM1:PersistentLevel.DirectionalLight_0.LightComponent0` and `...SkyLight_0.SkyLightComponent0`, then save.
   MCP save note: `SceneTools.save_actor` fails on these actors ("Asset does not exist: /Game/__ExternalActors__/..."), and `set_properties` alone does not dirty the package; add and remove a temporary actor tag (`ActorTools.add_tag`/`remove_tag`), then `AssetTools.save_assets` with an empty list (saves every dirty asset, so run it with no other unsaved work). Changed packages: Content/__ExternalActors__/RogueRigs/Maps/L_ArenaM1/7/NT/V5ATLGD6KXMEQFO2WZJWIB.uasset (DirectionalLight_0) and .../E/R7/FZQSIBGXQNH4S110A89HHQ.uasset (SkyLight_0).
4. Remove the provisional line under rule A12 of [[art-style-guide]] and in [[visual-style]].

- owner_agent: agent-bp

## Relations
- [[blueprints-world-index]] — parent index.
- [[art-style-guide]] — rule A12 (lifted shadows) is suspended while this is in place.
- [[visual-style]] — the look statement that mentions the shadows.
- [[ppv-xp-glow]] — the other lighting actor of the level, untouched.
- [[sys-cheats]] — the runtime halo light.
