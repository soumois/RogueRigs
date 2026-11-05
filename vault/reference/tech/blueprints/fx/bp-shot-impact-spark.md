---
id: bp-shot-impact-spark
description: "NS_ShotImpactSpark, the one-shot white-gold sparks where a player shot hits a robot or stops on a block; open for emitter values and spawn rule."
parent: blueprints-fx-index
kind: doc
---
# NS_ShotImpactSpark

- asset_path: /Game/RogueRigs/FX/NS_ShotImpactSpark (Niagara system, no Blueprint; task-432-02, commit 4d6554ab)
- parent_class: UNiagaraSystem
- purpose: small sparks at the impact of a miner or turret shot on a robot (shield included) or on any block the drone can dig: rock, ore, heal ore, special ore, barrel ([[art-style-guide]] A8, [[color-code]], blocks since [[task-437-shot-impact-on-walls]]). Never for robot shots.
- user_params: none. No light.
- emitters: `Sparks` only (made from the `Sparks` emitter of [[bp-muzzle-flash]], Fountain origin): CPU sim, local space, Loop Once 0.1 s, SpawnBurst_Instantaneous 6 particles, lifetime 0.1 to 0.2 s, AddVelocity cone axis +Z angle 80 deg, speed 150 to 400, gravity 0, drag 2, HDR white-gold (6, 4.6, 1.6), non-uniform size 1.2 to 1.8 x 6 to 9 uu, VelocityAligned. All values provisional: the owner tunes them in game (A4 of [[task-432-player-shot-impact-sparks]]: at most 8 sprites, 0.25 s).
- spawn: `ARRProjectile::ImpactSparkSystem`, constructor reference, through the Niagara pool; path and cap in [[sys-projectiles]]. Spawn rotation zero.
- components: none (Niagara asset). No collision, no events.
- logic_in_bp: none
- assets: engine DefaultSpriteMaterial (no new material).
- verified: compile UpToDate, 0 errors, 0 warnings, values read back; `RogueRigs.Weapons.ImpactSparkAsset` under `-RenderOffScreen`: a rifle hit plays it and it returns to the pool after 12 frames of 1/60 s (0.2 s).
- unverified: the look in game (nobody has seen it; the owner judges it, no capture in this task); cost in a horde (bounded by construction, no stress run).
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[sys-projectiles]] — the C++ property, hit rule and pool cap.
- [[sys-actor-pool]] — the Niagara pool that plays it.
- [[bp-muzzle-flash]] — the template it was made from.
- [[bp-dig-impact]] — the drone laser sparks it must not be confused with.
- [[task-432-player-shot-impact-sparks]] — the task that made it.
