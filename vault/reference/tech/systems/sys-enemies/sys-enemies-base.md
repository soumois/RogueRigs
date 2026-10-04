---
id: sys-enemies-base
description: "ARREnemy robot base as built: ASC and sets, controller-less movement, crowd and RVO, death, damage numbers, knockback, shared state-machine helpers for subclasses."
parent: sys-enemies
kind: doc
---
# Robot base (`ARREnemy`)

- responsibility: everything a robot has before its own behavior; subclasses implement `UpdateBehavior` (alive only) and may extend `HandleDeath` / `DropXP`.
- main_classes: `ARREnemy` (abstract `ACharacter`, `IAbilitySystemInterface`, `RREnemy.h/.cpp`): own `URRAbilitySystemComponent` with `URRHealthAttributeSet`, `URRMovementAttributeSet`, `URREnemyAttributeSet` ([[sys-gas-core]]); `URREnemyMovementComponent` ([[sys-flow-field]]). No controller (`AutoPossessAI` disabled, `bRunPhysicsWithNoController`, default movement mode at BeginPlay); RVO on, orient to movement, no walking off ledges. Capsule Overlap on `Projectile` ([[sys-projectiles]]).
- crowd (task-018-02, `Enemy|Crowd`): `bIgnoreOtherRobots` (true) makes `SetIgnoreOtherRobots` put `RRCollisionChannels::RobotMaskFilter` on the capsule body and move-ignore mask: robots pass through robots, everything else still collides. RVO is the only separation: `AvoidanceConsiderationRadius` 250 uu; BeginPlay writes `AvoidanceRadiusExpansion` 1.1 (0 = leave) and `AvoidanceLockTimeAfterAvoid` 0.3 s (negative = leave) to the world avoidance manager.
- death (single, `State.Dead`, [[sys-damage]]): `SetMovementActive(false)`, `DisableMovement`, collision off; linear shrink over `DeathShrinkDuration` (0.15 s); then `Stats.XPValue` pickups of `XPPickupClass` at the feet; destroyed. Notifies the run flow ([[sys-objectives]]).
- damage numbers (`EditAnywhere`, `Enemy|Damage Text`): one `FloatingTextClass` (`ARRFloatingDamageText`, [[sys-target-dummy]]) per hit, killing hit included, at capsule top + `TextSpawnHeight` (40) ± `TextSpawnJitter` (25); colors white / (1, 0.85, 0.45), sizes 60 / 90; `bShowDamageNumbers`. Code duplicated with the dummy (R6 of [[code-review-m1-plan]]).
- knockback: `ApplyKnockback(Velocity)` launches a living robot with XY × (1 − `GetKnockbackResistance()`); caller: the shotgun ([[sys-weapons]]).
- shared helpers (task-020-02, protected, for every state machine): `SetMovementActive(bool)` (off also drops input and cuts velocity), `IsTargetWithin(Miner, Range)` (alive, 2D, inclusive), `MarkStateStart` / `GetTimeInState` / `HasStateLasted(Duration)` on `StateStartTime` with `TimeTolerance` 1e-4 s, `ApplyDamageSpecTo(TargetASC, EffectClass, Damage)` (SetByCaller `Data.Damage`, source = own ASC so `AttackPower` scales it), `UpdateTelegraphBlink(Rate)` (hidden during the first half of each cycle since the state start), `SetMeshesVisible`. `ArrivalRangeRatio` (`Enemy|Movement`, base 0.9): arrival distance = engage range × ratio. `FindTarget()` = cached `RRWorld::FindLivingMiner` ([[sys-world-queries]]).
- entry_points: `LoadStatsRow`, `GetStats`, `GetHealthSet`, `GetEnemyMovement`, `IsDead`, `GetKnockbackResistance`, `ApplyKnockback`, `FindTarget`, `SetIgnoreOtherRobots`, `IsIgnoringOtherRobots`; virtual `UpdateBehavior`, `HandleDeath`, `DropXP`.
- data: `StatsRowName`, `XPPickupClass` (set by the Blueprint); CSV in [[sys-enemies]].
- depends_on: [[sys-gas-core]], [[sys-flow-field]], [[sys-world-queries]].
- implements: [[enemy-death]].
- gotchas: no native `BodyMesh` on the base: `BP_Chaser` and `ARRKamikaze` own one.

## Relations
- [[sys-enemies]] — parent index.
- [[sys-enemies-movement]] — the light movement the base also sets up.
