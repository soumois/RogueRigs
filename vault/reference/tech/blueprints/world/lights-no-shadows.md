---
id: lights-no-shadows
description: "Shadow suspension of the sun and sky light of L_ArenaM1, lifted by task-275-01 (shadows back as on Monday); history, per-actor flags kept."
parent: blueprints-world-index
kind: doc
---
# Lights without shadows (L_ArenaM1)

Lifted by task-275-01 (owner 2026-10-21): the sun and sky light cast shadows again, as on Monday (packages restored from 75610b21, read-back pending task-275-04, values in [[lights-dark-ambient]]). The per-actor flags of task-263-01 are NOT reverted (pending owner question, since the blob shadows of task-264 stay): with the sun shadows back, rocks and barrels cast, robots, miner and drone do not (blob discs only). The text below is history.

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
- Per-object CastShadow flags (meshes, ArenaGrid components) are on or off as before and are not part of this change (task-263-01 sets them since, section below).
- Decals: only the ground telegraph and explosion decals, which are gameplay marks, not shadows. No ShadowProxy mesh, no blob shadow actor found.

## Player light, the only shadow caster (task-263; removed in task-272-15)
Since task-272-15 (owner 2026-10-21) no light casts shadows: the sun and sky are shadowless and the player light is removed from BP_Miner. The grid flag `bBlocksCastShadows` on rocks and barrels stays on but has no visible effect now (no shadow-casting light). Revert: re-add the light with the values in [[bp-miner]]. The text below is the history of task-263.
Owner 2026-10-20 (reference game video): the only cast shadows are those of the player's light on the rock walls; rocks cast, nothing else. The sun and sky light above stay without shadows. Per-object flags set in C++ (task-263-01):
- Cast: grid rock piece ISMs, barrel HISMs and special ore (`ARRArenaGrid::bBlocksCastShadows`, default true, [[sys-grid]]). Ore and heal ore crystals stay off (`bDepositCrystalsCastShadow` false, task-237-01).
- Never cast: grid floor chunks; every primitive of robots (`ARREnemy`), pickups (`ARRPickup`), the drone (`ARRDrone`), the miner (`ARRMiner`), weapons and turrets (`ARRWeapon`) and projectiles (`ARRProjectile`), through `RRShadows::DisableCastShadows` in their `PostInitializeComponents` (C++ and Blueprint components, kept across pool reuse). Robots get a fake blob shadow instead (task-264).
- Blob shadows under actors (task-264): `URRBlobShadowComponent`, a flat translucent dark disc (MI_BlobShadow) under every robot, the miner and the drone, no cast shadow ([[sys-blob-shadow]]).
- Not covered (no C++ rule, flags as authored): objectives (console, shuttle, bonus chest), Niagara debris and effects, explosion FX (already off in C++).
- The light itself now exists (task-263-02, provisional, owner 2026-10-20, owner tunes by eye): PointLightComponent `PlayerLight` on BP_Miner, Movable, warm, Candelas, cast shadows on (first values 60 cd, radius 1400, height 300; current values since task-272-02: Z -15, radius 3000, 400 cd); values and tuning in [[bp-miner]]. DirectionalLight_0 and SkyLight_0 still cast no shadow (unchanged). Cost: a moving shadowed local light invalidates virtual shadow map pages each frame ([[perf-risks]]); reduce with a smaller AttenuationRadius or castShadows off.

## Revert (bring the shadows back)
1. Open L_ArenaM1, select DirectionalLight_0, Details, Light, Cast Shadows: tick it.
2. Select SkyLight_0, Details, Light, Cast Shadows: tick it.
3. Save the level (Ctrl+S) so the external actor files are written. Or by MCP: `ObjectTools.set_properties` with `{"castShadows": true}` on `...L_ArenaM1:PersistentLevel.DirectionalLight_0.LightComponent0` and `...SkyLight_0.SkyLightComponent0`, then save.
   MCP save note: `SceneTools.save_actor` fails on these actors ("Asset does not exist: /Game/__ExternalActors__/..."), and `set_properties` alone does not dirty the package; add and remove a temporary actor tag (`ActorTools.add_tag`/`remove_tag`), then `AssetTools.save_assets` with an empty list (saves every dirty asset, so run it with no other unsaved work). Changed packages: Content/__ExternalActors__/RogueRigs/Maps/L_ArenaM1/7/NT/V5ATLGD6KXMEQFO2WZJWIB.uasset (DirectionalLight_0) and .../E/R7/FZQSIBGXQNH4S110A89HHQ.uasset (SkyLight_0).
4. Remove the provisional line under rule A12 of [[art-style-guide]] and in [[visual-style]].

- owner_agent: agent-bp

## Relations
- [[blueprints-world-index]] — parent index.
- [[art-style-guide]] — rule A12 (lifted shadows), suspension lifted by task-275-01.
- [[visual-style]] — the look statement that mentions the shadows.
- [[ppv-xp-glow]] — the other lighting actor of the level, untouched.
- [[sys-cheats]] — the runtime halo light.
- [[sys-grid]] — `bBlocksCastShadows`: the rock and barrel shadow casters of the player light.
- [[lights-dark-ambient]] — same two actors: intensity and violet tint (task-265).
- [[bp-miner]] — carries the player light (task-263-02).
- [[perf-risks]] — cost of the moving shadowed light.
- [[sys-blob-shadow]] — the fake blob shadows under actors (task-264).
