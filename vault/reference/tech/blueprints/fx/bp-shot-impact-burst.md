---
id: bp-shot-impact-burst
description: "NS_ShotImpactBurstGreen, GreenSmall, White and M_ImpactBurstGreen/White: flat cartoon burst where a player shot hits; open for shapes, sizes, colours, sprite bound."
parent: blueprints-fx-index
kind: doc
---
# NS_ShotImpactBurst (green, green small, white)

- asset_path: /Game/RogueRigs/FX/NS_ShotImpactBurstGreen (rifle), NS_ShotImpactBurstGreenSmall (turrets), NS_ShotImpactBurstWhite (shotgun pellets); materials /Game/RogueRigs/FX/M_ImpactBurstGreen, M_ImpactBurstWhite (task-465-01, commits ece043ab, ffcc353a; task-465-03, commits deae8529 and the warmth fix after it). Niagara systems, no Blueprint.
- parent_class: UNiagaraSystem
- purpose: flat cartoon hit burst at each player shot impact on a robot or a block (rock, ore, barrel), after the owner reference `Art/Source/impact-burst/owner-ref-impact-burst-2026-11-05.png` (style example, keep): bright core, radial star of sharp spikes, glow, solid and outlined triangles ([[color-code]] player shot impacts row). Replaces [[bp-shot-impact-spark]] on every player shot.
- emitters (copied from [[bp-muzzle-flash]] NS_MuzzleFlash: CPU sim, local space, Loop Once 0.1 s, SpawnBurst_Instantaneous, VelocityAligned sprites, `Scale Alpha` = `Particles.NormalizedAge`):
  - `Sparks` (spikes): 5 sprites, each drawn as two sharp blades from the burst centre (forward and backward, uneven lengths); AddVelocity In Cone, axis +Z, Cone Angle 180, Inner 170 (horizontal ring; the module angle is a full angle), distribution and falloff off, speed 35 uu/s, drag 4; lifetime 0.12 to 0.18 s; size 9.8-15.4 x 50-90 uu.
  - `Core`: 3 sprites by `Engine.ExecIndex`: 0 core (42-50 uu, 0.12 s), 1 solid triangle, 2 outlined triangle (16-22 uu, 0.2 to 0.24 s, flying out in a random horizontal direction at 154 to 266 uu/s).
  - GreenSmall: same at 0.7x (spikes 6.9-10.8 x 35-63, core 29-35, triangles 11-16 uu, speeds x0.7), sized to the turret shot (task-465 Q4).
- sprite_count: 8 per impact, at most 0.24 s ([[art-style-guide]] A8 bound: at most 8 sprites, 0.25 s).
- material: Translucent, Unlit, two sided, used with Niagara sprites, depth test disabled (drawn over the robot it hits, otherwise half the burst hides inside the mesh), no texture. One Custom node, inputs UV and C = ParticleColor: B kind (below 0.25 spikes, 0.25-0.5 core, 0.5-0.75 solid triangle, else outlined), R tone variant, G seed (spike lengths, triangle angle), A progress. Spikes grow to full length by progress 0.18 and thin to nothing; core shrinks by 0.7 with a soft halo; triangles spin and shrink; glow fades before the quad edge.
- colours: Green ([[task-464-player-shot-colour]] greens): Main (0.45, 3.2, 0.12), Pale (2.8, 4.4, 1.9), Dark glow (0.02, 0.35, 0.01), Accent (0.2, 1.6, 0.1); spikes pale with green rim or green with pale centre line; triangles pale, black or green, outlines mid green or black. White: Pale (4.5, 4.2, 3.0) fill, Main (2.4, 1.45, 0.2) gold rims, 2 in 5 spikes gold with orange rim, Accent (2.6, 0.9, 0.08) orange outlines, Dark (0.9, 0.32, 0.03) warm orange glow. Black triangles kept from the reference (task-465 Q2). All provisional, owner tunes in game.
- spawn: `ARRProjectile::ImpactSparkSystem` per shot Blueprint through the Niagara pool, cap 24 ([[sys-projectiles]]): [[bp-projectile]] Green, BP_TurretBullet ([[bp-turret]]) GreenSmall, BP_ShotgunPellet ([[bp-shotgun]]) White. Spawn rotation zero. Robot shots none.
- components: none (Niagara asset). No collision, no events, no light.
- logic_in_bp: none
- verified: compile UpToDate, no errors; particle dump in -game (8 particles, horizontal spike velocities); slow-motion -game frames: rifle on a Heavy and on the rock wall, turret on Chasers, shotgun on Heavies (`Saved/Captures/task465_02_robot_compare.png`, `task465_02_rock_compare.png`, `task465_02_volley_compare.png`, `task465_03_check/turret.png`, `task465_03_shotgun2/crop.png`).
- unverified: a barrel hit in a frame; cost in a horde (bounded by the pool and 8 CPU sprites); black triangles barely visible on dark ground (owner's choice).
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[bp-shot-impact-spark]] — the sparks it replaced.
- [[bp-muzzle-flash]] — the system and material it was copied from.
- [[sys-projectiles]] — the C++ property, hit rule and pool cap.
- [[bp-projectile]] — rifle shot, green burst.
- [[bp-turret]] — turret bullet, small green burst.
- [[bp-shotgun]] — shotgun pellet, white burst.
- [[color-code]] — the player shot impacts row.
- [[task-465-shot-impact-burst]] — the task that made it.
