---
id: bp-shot-impact-spark
description: "NS_ShotImpactSpark (white-gold, shotgun) and NS_ShotImpactSparkGreen (rifle, turrets): one-shot sparks where a player shot hits; open for values and spawn rule."
parent: blueprints-fx-index
kind: doc
---
# NS_ShotImpactSpark

- asset_path: /Game/RogueRigs/FX/NS_ShotImpactSpark (Niagara system, no Blueprint; task-432-02, commit 4d6554ab); /Game/RogueRigs/FX/NS_ShotImpactSparkGreen (copy, task-464-01, commit 03a10507)
- parent_class: UNiagaraSystem
- purpose: small sparks at the impact of a miner or turret shot on a robot (shield included) or on any block the drone can dig: rock, ore, heal ore, special ore, barrel ([[art-style-guide]] A8, [[color-code]], blocks since [[task-437-shot-impact-on-walls]]). Never for robot shots.
- user_params: none. No light.
- emitters: `Sparks` only (made from the `Sparks` emitter of [[bp-muzzle-flash]], Fountain origin): CPU sim, local space, Loop Once 0.1 s, SpawnBurst_Instantaneous 6 particles, lifetime 0.1 to 0.2 s, AddVelocity cone axis +Z angle 80 deg, speed 150 to 400, gravity 0, drag 2, HDR white-gold (6, 4.6, 1.6), non-uniform size 1.2 to 1.8 x 6 to 9 uu, VelocityAligned. NS_ShotImpactSparkGreen: same emitter, InitializeParticle Color Mode Random Range, Link RGB, from (0.15, 2.4, 0.05) dark green to (3.2, 6.5, 1.8) pale white-green, so each spark gets its own tone ([[color-code]] player laser row). All values provisional: the owner tunes them in game (A4 of [[task-432-player-shot-impact-sparks]]: at most 8 sprites, 0.25 s).
- superseded (task-465): no shot Blueprint plays these sparks any more; the rifle, turret and shotgun shots play [[bp-shot-impact-burst]]. NS_ShotImpactSpark stays the C++ constructor default of `ARRProjectile::ImpactSparkSystem` and the reference of the test `RogueRigs.Weapons.ImpactSparkAsset`.
- spawn (before task-465): `ARRProjectile::ImpactSparkSystem`, constructor reference (NS_ShotImpactSpark, kept by BP_ShotgunPellet), through the Niagara pool; [[bp-projectile]] and BP_TurretBullet ([[bp-turret]]) set NS_ShotImpactSparkGreen; path and cap in [[sys-projectiles]]. Spawn rotation zero.
- components: none (Niagara asset). No collision, no events.
- logic_in_bp: none
- assets: engine DefaultSpriteMaterial (no new material).
- verified: compile UpToDate, 0 errors, 0 warnings, values read back; `RogueRigs.Weapons.ImpactSparkAsset` under `-RenderOffScreen`: a rifle hit plays it and it returns to the pool after 12 frames of 1/60 s (0.2 s).
- unverified: the look in game (nobody has seen it; the owner judges it, no capture in this task); cost in a horde (bounded by construction, no stress run).
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[bp-shot-impact-burst]] — the burst that replaced it.
- [[sys-projectiles]] — the C++ property, hit rule and pool cap.
- [[sys-actor-pool]] — the Niagara pool that plays it.
- [[bp-muzzle-flash]] — the template it was made from.
- [[bp-dig-impact]] — the drone laser sparks it must not be confused with.
- [[task-432-player-shot-impact-sparks]] — the task that made it.
- [[task-464-player-shot-colour]] — the green copy.
- [[bp-projectile]] — rifle shot, green sparks.
- [[bp-turret]] — turret bullet, green sparks.
