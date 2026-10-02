---
id: sys-enemies
description: "Robots as built: ARREnemy base (ASC, sets, EnemyStats.csv, single death, shrink, XP drop) and ARRChaser state machine; tunables and what Blueprints set."
parent: systems-index
kind: doc
---
# Enemies

- responsibility: the robot pawns: stats, damage intake, death and XP drop, and each robot's decisions. Spawning: not yet.
- main_classes (`Source/RogueRigs/Enemies/`):
  - `ARREnemy` (abstract `ACharacter`, `IAbilitySystemInterface`): own `URRAbilitySystemComponent` with `URRHealthAttributeSet`, `URRMovementAttributeSet`, `URREnemyAttributeSet` ([[sys-gas-core]]); `URREnemyMovementComponent` ([[sys-flow-field]]). No controller (`AutoPossessAI` disabled, `bRunPhysicsWithNoController`, default movement mode set at BeginPlay); RVO avoidance on, orient to movement, no walking off ledges. Capsule Overlap on `Projectile` ([[sys-projectiles]]). `FindTarget()` = first living `ARRMiner` (cached).
  - Death (single, `State.Dead` from the health set, [[sys-damage]]): movement off, velocity cut, `DisableMovement`, collision off; scale shrinks linearly to 0 over `DeathShrinkDuration` (0.15 s); then `Stats.XPValue` pickups of `XPPickupClass` spawn at the feet recorded at death, and the robot is destroyed.
  - `ARRChaser` (Blueprintable): `GetState()` → `ERRChaserState` Chase, WindUp, Attack, Recover, Dead. Chase (movement on) → WindUp when the target is alive within `AttackRange` (2D, centers): movement off, meshes blink at `WindUpFlashRate` (12 Hz) for `WindUp` → Attack (one frame): if still in range, `URRGE_DamageFlat` from its own ASC, SetByCaller `Data.Damage` = `Damage` (× `AttackPower`) → Recover (movement off) until 1/`AttackRate` after the attack → Chase. Arrival distance = `AttackRange` × `ArrivalRangeRatio` (0.9 → 108 uu). Measured cadence in contact: 0.25 s, then every 1.267 s (recover 1 s + 1 frame + wind-up). Default capsule 24 × 62 (≈ 0.7 × default character).
- editable (`EditDefaultsOnly`): `StatsRowName` (chaser: `Chaser`), `XPPickupClass` (none by default: the Blueprint sets `BP_PickupXP`), `DeathShrinkDuration`; chaser `DamageEffectClass` (`URRGE_DamageFlat`), `ArrivalRangeRatio`, `WindUpFlashRate`. Mesh and eye: Blueprint subclass.
- entry_points: `ARREnemy::LoadStatsRow`, `GetStats`, `GetHealthSet`, `GetEnemyMovement`, `IsDead`; `ARRChaser::GetState`, `GetLastAttackTime`, `GetAttackCount`.
- data: `Content/RogueRigs/Data/Tables/EnemyStats.csv` (`FRREnemyStatsRow`): MaxHealth, MoveSpeed, Damage, AttackRate, AttackRange, WindUp, XPValue, KnockbackResistance; row `Chaser` 30, 450, 10, 1, 120, 0.25, 1, 0. MaxHealth, Health and MoveSpeed become base values; the rest stay CSV constants. KnockbackResistance is stored only.
- depends_on: [[sys-gas-core]], [[sys-damage]], [[sys-flow-field]], [[sys-pickups]], [[sys-player]].
- implements: [[enemy-chaser]], [[enemy-death]] (M1 shrink and drop).
- gotchas: the CSV is read on every spawn (cache it if spawns get frequent). The flash hides meshes (no material yet). Damage refused by i-frames still counts as an attack (Recover runs). Tests: [[sys-automation-tests]].

## Relations
- [[systems-index]] — parent index.
- [[sys-enemy-ai]] — the framework decision the chaser state machine follows.
- [[sys-flow-field]] — the movement component the robots use.
- [[sys-pickups]] — the XP pickup dropped on death.
