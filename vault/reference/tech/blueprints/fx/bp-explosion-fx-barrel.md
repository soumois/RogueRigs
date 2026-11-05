---
id: bp-explosion-fx-barrel
description: "BP_ExplosionFX_Barrel, the barrel and kamikaze explosion visual: BP_ExplosionFX child with the Niagara burst and ground shockwave ring; open to see what those rows play."
parent: blueprints-fx-index
kind: doc
---
# BP_ExplosionFX_Barrel

- asset_path: /Game/RogueRigs/Blueprints/FX/BP_ExplosionFX_Barrel (class path `/Game/RogueRigs/Blueprints/FX/BP_ExplosionFX_Barrel.BP_ExplosionFX_Barrel_C`, referenced by the `Barrel` and `Kamikaze` rows of ExplosionStats.csv)
- parent_class: BP_ExplosionFX ([[bp-explosion-fx]]), itself ARRExplosionFX
- purpose: the barrel and kamikaze explosion visual: the orange sphere of the parent plus a Niagara burst and a ground ring, owner request 2026-10-16 ("an explosion particle part and an explosion circle, reusable for the kamikaze"). Built by task-197-06.
- components: inherited only (root Mesh: engine Sphere, MI_GB_Explosion); none added.
- exposed_defaults (class defaults, read back): `BurstSystem` = /Game/RogueRigs/FX/NS_BarrelExplosion ([[bp-barrel-explosion]]); `ShockwaveMaterial` = /Game/RogueRigs/FX/MI_ExplosionShockwave ([[bp-explosion-shockwave]]); Duration 0.25 and FadeSeconds 0.25 inherited. Radius and hold are passed from C++ at spawn; contract in [[sys-explosions]].
- events: none
- logic_in_bp: none
- assets: NS_BarrelExplosion, MI_ExplosionShockwave.
- kamikaze: the `Kamikaze` row plays this class (task-434-01, reuse accepted, owner 2026-11-05, "Oui, lot 1"; row change, no second child). Same emitters and particle counts, every size from its own visual radius passed at spawn, no own tuning until the owner has seen it in game (provisional, owner 2026-11-05, "Ok, tout par défaut"). A separate look would need a second child.
- verified: Blueprint compiled, saved, parent and both defaults read back. Not verified: the in-game look and that the subsystem spawns this class (no MCP route to call the subsystem; the in-game check is task-197-07). Task-434-01: both defaults read back again; the kamikaze look not seen by the agent (no capture, the owner validates in game).
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[bp-explosion-fx]] — parent Blueprint.
- [[sys-explosions]] — the C++ class and the data row that point to it.
- [[bp-barrel-explosion]] — the burst it plays.
- [[bp-explosion-shockwave]] — the ring material it sets.
