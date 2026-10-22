---
id: bp-shooter
description: "BP_Shooter and BP_ShooterProjectile, Blueprints of ARRShooter and its shot: Meshy mesh SM_Shooter_Meshy on BodyMesh (since task-283-09), muzzle at the cannon tip, red laser and shot."
parent: blueprints-enemies-index
kind: doc
---
# BP_Shooter

- asset_path: /Game/RogueRigs/Blueprints/Enemies/BP_Shooter (projectile: /Game/RogueRigs/Blueprints/Enemies/BP_ShooterProjectile)
- parent_class: ARRShooter (/Script/RogueRigs.RRShooter); projectile: ARREnemyProjectile (/Script/RogueRigs.RREnemyProjectile)
- purpose: the ranged robot; visuals only, behaviour, laser and shots in C++ ([[sys-enemies-shooter]]).
- components_current (task-283-09): inherited C++ components (capsule 34 x 88, MuzzlePoint, LaserBeam) plus BodyMesh only: StaticMesh /Game/RogueRigs/Art/Shooter/SM_Shooter_Meshy, parent CollisionCylinder (capsule, half height 88), RelativeLocation (0, 0, -88) so the mesh origin sits on the floor (C++ adds no mesh parent for the shooter), scale 1, no OverrideMaterials (slots body = MI_Shooter_Body, robot_eye = MI_RobotEye_Shooter). EyeLeft, EyeRight, LogoPlate, UpperArm, ElbowJoint, Cannon removed. MuzzlePoint relative (45.7, 50.4, -17.4) = reported cannon tip (45.7, 50.4, 70.6 from the floor origin) minus 88. The next line describes the grey-box state before 283-09.
- components_before_283_09: inherited C++ components (capsule 34 x 88 = miner size, MuzzlePoint, LaserBeam) plus rigid primitives, all NoCollision, no dynamic shadow, attached to the capsule: BodyMesh (engine Cylinder, scale 0.6, 0.6, 1.5, relative (0, 0, -10), MI_GB_Shooter), EyeLeft / EyeRight (engine Sphere 0.13 at (27, -/+11, 38), MI_GB_Eye), LogoPlate (engine Cube 0.04, 0.32, 0.32 at (30, 0, 5), MI_GB_Chaser yellow), UpperArm (Cylinder 0.14, 0.14, 0.6 at (0, 41, 0)), ElbowJoint (Sphere 0.2 at (0, 41, -30)), Cannon (Cylinder 0.18, 0.18, 0.72, pitch 90 so it points forward, at (36, 41, -30)); all arm parts MI_GB_Shooter. Projectile: ShotMesh (engine Sphere 0.15 = 15 uu, MI_GB_Eye red, NoCollision).
- exposed_defaults (MuzzlePoint now see components_current): XPPickupClass = BP_PickupXP_C; ProjectileClass = BP_ShooterProjectile_C; MuzzlePoint moved to relative (72, 41, -30) (cannon tip, 58 uu above the floor); LaserBeam StaticMesh = engine Cube (100 uu, matches LaserMeshSize), material MI_GB_Laser (red), visibility and transform untouched (C++ drives them); all other C++ defaults kept.
- events: none (OnAimStart / OnAimEnd / OnFire unused).
- logic_in_bp: none
- assets: /Game/RogueRigs/Materials/GreyBox/MI_GB_Shooter (new, acid green Color 0.45, 1, 0.05), MI_GB_Eye, MI_GB_Chaser, MI_GB_Laser (reused); instances of M_GreyBox.
- verified: PIE with the cheat route (bInvulnerable, spawner TrickleDuration 1 / PackInterval 2 set on the PIE instance only): 25 shooters among chasers and kamikazes, held 900 to 1000 uu from the miner, LaserBeam bVisible true with scale (8.6, 0.04, 0.04) while aiming then false, ShooterProjectile actors appeared (one at z = 60 next to the miner) and only 2 were alive among 25 shooters; no game log error. Shots stopping on rock was not observed directly (covered by the C++ tests).
- gotcha: the level-up screen pauses the game; during checks send `ChooseUpgrade 0` on the cheat CDO ([[sys-cheats]]) or time stands still. Keep the editor in front (no shell commands) while PIE runs.
- readability: task-266-01 lowered MI_GB_Shooter Color to (0.3, 0.68, 0.035); task-275-02 (owner 2026-10-21) restored (0.45, 1, 0.05); see [[bp-robot-readability]].
- eye_278_10 (task-278-10, 2026-10-21): before = EyeLeft and EyeRight OverrideMaterials [MI_GB_Eye]; the EventGraph has no SetMaterial (only empty EventBeginPlay, EventActorBeginOverlap, EventTick). After = both eyes [/Game/RogueRigs/Materials/Robots/MI_RobotEye_GreyBox]. Nothing else changed. The eyes now glow red: EyeColor (1, 0.05, 0.05) x GlowAmount 23, provisional, owner 2026-10-21, task-278. MI_GB_Eye itself unchanged; BP_ShooterProjectile ShotMesh keeps MI_GB_Eye (untouched). Read back, compiled without LogBlueprint error, saved. Revert: `git checkout 2ad7698a -- Content/RogueRigs/Blueprints/Enemies/BP_Shooter.uasset`. Not verified: PIE, render, glow look.
- task-283-09 (2026-10-22): before = BodyMesh engine Cylinder scale (0.6, 0.6, 1.5) at z -10 override MI_GB_Shooter, plus EyeLeft/EyeRight (override MI_RobotEye_GreyBox), LogoPlate, UpperArm, ElbowJoint, Cannon; MuzzlePoint (72, 41, -30). After = components_current. Mesh imported from Art/Meshes/shooter-meshy/shooter_meshy_game.fbx (cm, no collision, Nanite off, 1 LOD, no material or texture imported); bounds read back min (-29.23, -66.39, 0) max (53.36, 65.15, 176) = report (82.6 x 131.5 x 176, Y mirrored), 7263 triangles; height 176 = capsule 2 x 88 (provisional, owner 2026-10-22). C++ and tests grepped: nothing names the removed components; the EventGraph (empty EventBeginPlay, EventActorBeginOverlap, EventTick) read back unchanged. Compiled without LogBlueprint error for BP_Shooter, saved. Revert: `git checkout f47bc954 -- Content/RogueRigs/Blueprints/Enemies/BP_Shooter.uasset`, then delete SM_Shooter_Meshy (new file). Not verified: PIE, render, laser/muzzle alignment in game, mesh facing.
- owner_agent: agent-bp

## Relations
- [[bp-robot-eye-material]] — the eye material of the old grey-box eyes.
- [[bp-robot-meshy-material]] — MI_Shooter_Body and MI_RobotEye_Shooter on the new mesh.
- [[bp-robot-readability]] — the reverted colour tweak against burn.
- [[blueprints-index]] — parent index.
- [[sys-enemies-shooter]] — the C++ class.
- [[bp-pickup-xp]] — the pickup it drops.
- [[bp-enemy-spawner]] — the spawner that creates it.
- [[bp-chaser]] — the sibling robot whose primitive body it follows.
