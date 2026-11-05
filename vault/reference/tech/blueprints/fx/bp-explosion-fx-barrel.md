---
id: bp-explosion-fx-barrel
description: "BP_ExplosionFX_Barrel, the barrel and kamikaze explosion visual: BP_ExplosionFX child with the Niagara burst and ground shockwave ring; open to see what those rows play."
parent: blueprints-fx-index
kind: doc
---
# BP_ExplosionFX_Barrel

- asset_path: /Game/RogueRigs/Blueprints/FX/BP_ExplosionFX_Barrel (class path `/Game/RogueRigs/Blueprints/FX/BP_ExplosionFX_Barrel.BP_ExplosionFX_Barrel_C`, referenced by the `Barrel` and `Kamikaze` rows of ExplosionStats.csv)
- parent_class: BP_ExplosionFX ([[bp-explosion-fx]]), itself ARRExplosionFX
- purpose: the barrel and kamikaze explosion visual: a sphere, a Niagara burst and a ground ring, owner request 2026-10-16 ("an explosion particle part and an explosion circle, reusable for the kamikaze"). Built by task-197-06. Since task-446 (provisional, owner 2026-11-05, "refaire tout l'explosion en style cartoon") all three parts have the cartoon look of the robot death flash: flat cel bands, jagged edges, white and yellow core, orange and red-orange outside, dark red at the end.
- components: inherited only (root Mesh: engine Sphere); none added. The inherited Mesh has `OverrideMaterials` = /Game/RogueRigs/FX/M_CartoonExplosionSphere (task-446-02, was MI_GB_Explosion, which the parent keeps).
- exposed_defaults (class defaults, read back): `BurstSystem` = /Game/RogueRigs/FX/NS_BarrelExplosion ([[bp-barrel-explosion]]); `ShockwaveMaterial` = /Game/RogueRigs/FX/MI_ExplosionShockwave ([[bp-explosion-shockwave]]); Duration 0.25, FadeSeconds 0.25 and FadeParameterName `Opacity` inherited. Radius and hold are passed from C++ at spawn; contract in [[sys-explosions]].
- sphere_material: M_CartoonExplosionSphere, Translucent, Unlit, one-sided, one Custom node, no texture. Inputs: VertexNormalWS, CameraVectorWS, WorldPosition, ObjectPositionWS, scalar `Opacity` (default 1). The projected radius r = sin of the angle between normal and view (0 centre, 1 silhouette); band edges jagged by triangle waves of the azimuth around world Z (same waves as M_CartoonBlastCore, [[bp-robot-death-puff]]). Bands from the centre: white (2.0, 1.95, 1.7) to about 0.5, yellow (1.5, 1.0, 0.08) to 0.7, orange (1.1, 0.33, 0.02) to 0.86, red-orange (0.7, 0.09, 0.013) to the edge. The outer edge is jagged only between 0.956 and 1.0 of the mesh radius, so the visible part stays above the collision radius (visual radius = row radius x 1.05). Fade: progress = 1 - `Opacity` (0 during growth and hold); each band cools one step towards dark red (0.17, 0.016, 0.012) and a jagged hole opens from the centre, leaving a hollow ring; alpha also x saturate(`Opacity` x 6), so nothing is left at `Opacity` 0.
- events: none
- logic_in_bp: none
- assets: NS_BarrelExplosion, MI_ExplosionShockwave, M_CartoonExplosionSphere.
- kamikaze: the `Kamikaze` row plays this class (task-434-01, reuse accepted, owner 2026-11-05, "Oui, lot 1"; row change, no second child). Same emitters and particle counts, every size from its own visual radius passed at spawn, no own tuning until the owner has seen it in game (provisional, owner 2026-11-05, "Ok, tout par défaut"). A separate look would need a second child.
- verified: Blueprint compiled, saved, parent and both defaults read back. Not verified: the in-game look and that the subsystem spawns this class (no MCP route to call the subsystem; the in-game check is task-197-07). Task-434-01: both defaults read back again; the kamikaze look not seen by the agent (no capture, the owner validates in game). Task-446-02: material compiles, Blueprint compiles and saves, `OverrideMaterials` and the defaults above read back; the look not seen in game (owner validates, no capture), only a formula preview outside the engine.
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[bp-explosion-fx]] — parent Blueprint.
- [[sys-explosions]] — the C++ class and the data row that point to it.
- [[bp-barrel-explosion]] — the burst it plays.
- [[bp-explosion-shockwave]] — the ring material it sets.
- [[bp-robot-death-puff]] — the cartoon flash its three parts follow.
