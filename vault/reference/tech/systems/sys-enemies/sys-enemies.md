---
id: sys-enemies
description: "Robots as built, index: ARREnemy base, shared state-machine helpers, light movement, chaser, kamikaze, shooter and heavy; EnemyStats.csv. Open a child for detail."
parent: systems-index
kind: doc
---
# Enemies

- responsibility: the robot pawns: stats, damage intake, death and XP drop, and each robot's decisions. Spawning: [[sys-spawner]].
- main_classes: `Source/RogueRigs/Enemies/`: `ARREnemy` (abstract base), `URRRobotMovementComponent`, `ARRChaser`, `ARRKamikaze`, `ARRShooter`, `ARRHeavy`, `ARRShieldChaser` (one child node each below).
- entry_points: listed per child.
- data: `Content/RogueRigs/Data/Tables/EnemyStats.csv` (`FRREnemyStatsRow`): MaxHealth, MoveSpeed, Damage, AttackRate, AttackRange, WindUp, XPValue, KnockbackResistance, PreferredDistance, PreferredDistanceTolerance, ProjectileSpeed (these three: ranged robots, 0 otherwise), Shield (task-028-01: shield points before health, 0 otherwise); MoveSpeed −30 % on every row (owner 2026-10-06 second test, task-031-02: chaser and shield chaser 202, kamikaze 168, shooter 168, heavy 112; was 288 / 240 / 240 / 160 after −20 % in task-030-01, 360 / 300 / 300 / 200 before); row `Chaser` 30, 202, 10, 1, 120, 0.1, 1, 0; row `Kamikaze` 100, 168, 0, 0, 150, 0.5, 1, 0 (owner tuning 2026-10-02, task-012-01; AttackRange = arm range, WindUp = fuse since task-021-01; Damage, AttackRate unused); row `Shooter` 40, 168, 12, 0.5, 960, 0.6, 2, 0.2, 800, 200, 1200 ([[enemy-shooter]]); row `Heavy` 300, 112, 25, 0.4, 400, 0.8, 5, 0.8, 0, 0, 0 ([[enemy-heavy]]; AttackRange = slam radius, 220 before task-028-01; WindUp = telegraph); row `ShieldChaser` = the chaser row with XPValue 2 and Shield 30 ([[enemy-chaser]] shield variant); Shield 0 on the other rows. MaxHealth, Health, MoveSpeed and Shield become ASC base values; the rest stay CSV constants in `Stats`.
- depends_on: [[sys-gas-core]], [[sys-damage]], [[sys-flow-field]], [[sys-pickups]], [[sys-player]], [[sys-explosions]], [[sys-world-queries]], [[sys-grid]], [[sys-projectiles]].
- implements: [[enemy-chaser]], [[enemy-kamikaze]], [[enemy-shooter]], [[enemy-heavy]], [[enemy-death]] (M1 shrink and drop).
- gotchas: the CSV is parsed once and cached by `FRRBaseStatsLoader::FindRow` (re-parsed when the file timestamp changes; R3 of [[code-review-m1-plan]]). Tests: [[sys-automation-tests]].

## Relations
- [[systems-index]] — parent index.
- [[sys-enemies-base]] — `ARREnemy`: ASC, no controller, crowd and RVO, death, damage numbers, knockback, shared state-machine helpers. Open to write a new robot.
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
