---
id: bp-player-laser-shot-material
description: "M_PlayerLaserShot, the unlit green material of the burst rifle and turret shots: pale core, bright green body, dark green edge; open for values."
parent: blueprints-fx-index
kind: doc
---
# M_PlayerLaserShot

- asset_path: /Game/RogueRigs/FX/M_PlayerLaserShot (material, no Blueprint; task-464-01, commit 03a10507)
- parent_class: UMaterial
- purpose: the green look of the player laser shots ([[color-code]] player laser row, owner reference `Art/Source/muzzle-flash/owner-ref-green-laser-muzzle-2026-11-05.webp`) on the small sphere `ShotMesh` of [[bp-projectile]] (rifle) and BP_TurretBullet ([[bp-turret]]). Shotgun pellets keep MI_GB_Shot white-gold ([[bp-shotgun]]).
- components: none (material). Opaque, Unlit, emissive only.
- exposed_defaults: VectorParameter `CoreColor` (0.75, 1, 0.5), `BodyColor` (0.12, 1, 0.02), `EdgeColor` (0.02, 0.28, 0.01); ScalarParameter `Glow` 3. All provisional, owner 2026-11-05.
- events: none
- logic_in_bp: one Custom node (`LaserShotTones`, float3): F = Fresnel (exponent 1, base reflect fraction 0); F below 0.15 blends Core to Body, above it Body to Edge; times Glow. Seen from the game camera the shot is a green dash with a pale centre and a darker green rim.
- assets: none (no texture).
- verified: -game frames `Saved/Captures/task464_01_check/zoom.png`, `task464_02_check/turret.png` (not committed): green with pale centre and darker rim, about the size of the red robot shot. Unverified: owner judgement of the tones.
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[bp-projectile]] — the rifle shot that uses it.
- [[bp-turret]] — BP_TurretBullet uses it.
- [[color-code]] — the colour row it implements.
- [[task-464-player-shot-colour]] — the task that made it.
