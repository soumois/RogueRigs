---
id: bp-shooter
description: "BP_Shooter and BP_ShooterProjectile, Blueprints of ARRShooter and its shot: acid-green body, elbow cannon, red laser and shot; open for meshes, muzzle and PIE results."
parent: blueprints-index
kind: doc
---
# BP_Shooter

- asset_path: /Game/RogueRigs/Blueprints/Enemies/BP_Shooter (projectile: /Game/RogueRigs/Blueprints/Enemies/BP_ShooterProjectile)
- parent_class: ARRShooter (/Script/RogueRigs.RRShooter); projectile: ARREnemyProjectile (/Script/RogueRigs.RREnemyProjectile)
- purpose: the ranged robot; visuals only, behaviour, laser and shots in C++ ([[sys-enemies-shooter]]).
- components: inherited C++ components (capsule 34 x 88 = miner size, MuzzlePoint, LaserBeam) plus rigid primitives, all NoCollision, no dynamic shadow, attached to the capsule: BodyMesh (engine Cylinder, scale 0.6, 0.6, 1.5, relative (0, 0, -10), MI_GB_Shooter), EyeLeft / EyeRight (engine Sphere 0.13 at (27, -/+11, 38), MI_GB_Eye), LogoPlate (engine Cube 0.04, 0.32, 0.32 at (30, 0, 5), MI_GB_Chaser yellow), UpperArm (Cylinder 0.14, 0.14, 0.6 at (0, 41, 0)), ElbowJoint (Sphere 0.2 at (0, 41, -30)), Cannon (Cylinder 0.18, 0.18, 0.72, pitch 90 so it points forward, at (36, 41, -30)); all arm parts MI_GB_Shooter. Projectile: ShotMesh (engine Sphere 0.15 = 15 uu, MI_GB_Eye red, NoCollision).
- exposed_defaults: XPPickupClass = BP_PickupXP_C; ProjectileClass = BP_ShooterProjectile_C; MuzzlePoint moved to relative (72, 41, -30) (cannon tip, 58 uu above the floor); LaserBeam StaticMesh = engine Cube (100 uu, matches LaserMeshSize), material MI_GB_Laser (red), visibility and transform untouched (C++ drives them); all other C++ defaults kept.
- events: none (OnAimStart / OnAimEnd / OnFire unused).
- logic_in_bp: none
- assets: /Game/RogueRigs/Materials/GreyBox/MI_GB_Shooter (new, acid green Color 0.45, 1, 0.05), MI_GB_Eye, MI_GB_Chaser, MI_GB_Laser (reused); instances of M_GreyBox.
- verified: PIE with the cheat route (bInvulnerable, spawner TrickleDuration 1 / PackInterval 2 set on the PIE instance only): 25 shooters among chasers and kamikazes, held 900 to 1000 uu from the miner, LaserBeam bVisible true with scale (8.6, 0.04, 0.04) while aiming then false, ShooterProjectile actors appeared (one at z = 60 next to the miner) and only 2 were alive among 25 shooters; no game log error. Shots stopping on rock was not observed directly (covered by the C++ tests).
- gotcha: the level-up screen pauses the game; during checks send `ChooseUpgrade 0` on the cheat CDO ([[sys-cheats]]) or time stands still. Keep the editor in front (no shell commands) while PIE runs.
- owner_agent: agent-bp

## Relations
- [[blueprints-index]] — parent index.
- [[sys-enemies-shooter]] — the C++ class.
- [[bp-pickup-xp]] — the pickup it drops.
- [[bp-enemy-spawner]] — the spawner that creates it.
- [[bp-chaser]] — the sibling robot whose primitive body it follows.
