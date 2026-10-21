---
id: sys-enemies
description: "Robots as built, index: ARREnemy base, shared state-machine helpers, light movement, chaser, kamikaze, shooter and heavy; EnemyStats.csv. Open a child for detail."
parent: systems-enemies-index
kind: doc
---
# Enemies

- responsibility: the robot pawns: stats, damage intake, death and XP drop, and each robot's decisions. Spawning: [[sys-spawner]].
- main_classes: `Source/RogueRigs/Enemies/`: `ARREnemy` (abstract base), `URRRobotMovementComponent`, `ARRChaser`, `ARRKamikaze`, `ARRShooter`, `ARRHeavy`, `ARRShieldChaser` (one child node each below), `URRRobotSeparationSubsystem` (crowd separation, in [[sys-enemies-base]]).
- entry_points: listed per child.
- data: `Content/RogueRigs/Data/Tables/EnemyStats.csv` (`FRREnemyStatsRow`): MaxHealth, MoveSpeed, Damage, AttackRate, AttackRange, WindUp, XPValue, KnockbackResistance, PreferredDistance, PreferredDistanceTolerance, ProjectileSpeed (these three: ranged robots, 0 otherwise), Shield (task-028-01: shield points before health, 0 otherwise); MoveSpeed 180 on every row except Heavy 153 (task-047-01; 180 on every row from owner, task-033; was 182 / 151 / 151 / 101 for chaser and shield chaser / kamikaze / shooter / heavy after task-032-01, 202 / 168 / 168 / 112 after task-031-02, 288 / 240 / 240 / 160 after task-030-01, 360 / 300 / 300 / 200 before); row `Chaser` 30, 180, 10, 1, 120, 0.1, 1, 0; row `Kamikaze` 100, 180, 0, 0, 250, 0.65, 1, 0 (arm range 250 and fuse 0.65 since task-034-01, were 150 and 0.5 from owner tuning 2026-10-02, task-012-01; AttackRange = arm range, WindUp = fuse since task-021-01; Damage, AttackRate unused); row `Shooter` 40, 180, 12, 0.5, 960, 0.6, 2, 0.2, 800, 200, 1200 ([[enemy-shooter]]); row `Heavy` 300, 153, 25, 0.5, 289, 0.6, 5, 0.8, 0, 0, 0 ([[enemy-heavy]]; AttackRange = slam radius, 289 since task-052-01, 340 after task-039-01, 220 before task-028-01; WindUp = telegraph, 0.8 and AttackRate 0.4 before task-032-01); row `ShieldChaser` = the chaser row with XPValue 2 and Shield 30 ([[enemy-chaser]] shield variant); Shield 0 on the other rows. MaxHealth, Health, MoveSpeed and Shield become ASC base values; the rest stay CSV constants in `Stats`.
- depends_on: [[sys-gas-core]], [[sys-damage]], [[sys-flow-field]], [[sys-pickups]], [[sys-player]], [[sys-explosions]], [[sys-world-queries]], [[sys-grid]], [[sys-projectiles]].
- implements: [[enemy-chaser]], [[enemy-kamikaze]], [[enemy-shooter]], [[enemy-heavy]], [[enemy-death]] (M1 shrink and drop).
- shadows (task-263-01): `ARREnemy::PostInitializeComponents` calls `RRShadows::DisableCastShadows` (`World/RRShadows.h`): every primitive of every robot (C++ and Blueprint meshes of BP_Chaser, BP_ShieldChaser, BP_Kamikaze, BP_Shooter, BP_Heavy) casts no shadow, once per actor, kept across pool reuse (the pool never touches CastShadow); the fake blob shadow is task-264 ([[lights-no-shadows]]).
- gotchas: the CSV is parsed once and cached by `FRRBaseStatsLoader::FindRow` (re-parsed when the file timestamp changes; R3 of [[code-review-m1-plan]]). Tests: [[sys-automation-tests]].

## Relations
- [[systems-index]] — parent index.
- [[sys-enemies-base]] — `ARREnemy`: ASC, no controller, crowd separation subsystem, terrain and hit capsules, death, damage numbers, knockback, shared state-machine helpers. Open to write a new robot.
- [[sys-enemies-movement]] — light movement: cached floor, light attached parts, far-movement tick LOD.
- [[sys-enemies-chaser]] — chaser state machine and tunables.
- [[sys-enemies-kamikaze]] — kamikaze state machine, hover and core visuals, fuse and explosion.
- [[sys-enemies-shooter]] — shooter state machine, ring keeping, sight and screen gates, laser, Blueprint events, reposition search.
- [[sys-enemies-heavy]] — heavy state machine, telegraphed slam, corridor capsule, Blueprint events and getters.
- [[sys-enemies-shield-chaser]] — shield chaser: shield before health, break event and getters, director mix after 1:00.
- [[sys-enemy-ai]] — the framework decision the state machines follow.
- [[sys-flow-field]] — the movement component the robots use.
- [[sys-pickups]] — the XP pickup dropped on death.
- [[sys-spawner]] — the actor that spawns the robots.
- [[sys-explosions]] — the queue the kamikaze explodes through.
