---
id: code-review-m1-gameplay
description: "M1 review findings F1 to F7: robot duplication, projectile friend-or-foe, weapon owner coupling, world lookups, reachable-cell searches."
parent: code-review-m1
kind: doc
---
# Review findings — gameplay code

Numbers are line counts; `F<n>` ids are used by [[code-review-m1-plan]].

## Robots (`Enemies/`)
- F1 duplicated state-machine plumbing. `ARRChaser::EnterState` and `ARRKamikaze::EnterState` both repeat stop-movement (`SetMovementEnabled`, `ConsumeMovementInputVector`, `StopMovementImmediately`, also in `ARREnemy::HandleDeath`); `RRChaser::TimeTolerance` and `RRKamikaze::TimeTolerance` are the same constant; `IsTargetInRange` / `IsTargetInArmRange` are the same 2D test; `StateStartTime` and `ArrivalRangeRatio` exist in both. The shooter needs all of them plus a telegraph blink (`ARRChaser::UpdateFlash`) and a damage spec to the miner (`ARRChaser::ApplyAttack`).
- F2 tuning split: chaser range and wind-up come from `EnemyStats.csv`, kamikaze `ArmRange` / `FuseTime` are `UPROPERTY` defaults while its CSV columns stay 0. Two places to tune one concept.
- F3 `URREnemyMovementComponent` (flow-field steering, in `FlowField/`) vs `URRRobotMovementComponent` (CharacterMovement subclass, in `Enemies/`): names do not say which is which.

## Projectiles
- F4 `ARRProjectile::OnSphereBeginOverlap` hits the first actor with any ASC except owner and instigator. An enemy shot would damage robots; `GetCarriedDamage` reads only `URRWeaponAttributeSet::Damage`, and `OnProjectileStop` always damages barrels. Spawning (deferred spawn, `InitProjectile`, `FinishSpawning`) lives inside `ARRWeapon::FireOnce` only.

## Weapons
- F5 `ARRWeapon` is bound to `ARRMiner` (`OwnerMiner`, `GetAimDirection`, `GetShotOrigin`, non-virtual). A turret needs its own aim and origin. `ARRShotgun::ResolveGrid` repeats the grid lookup.

## World queries
- F6 first-grid lookup copied 6 times (`ARRMiner::FindArenaGrid`, `ARRShotgun::ResolveGrid`, `URRExplosionSubsystem::ResolveGrid`, `URRFlowFieldSubsystem::ResolveGridAndGoal`, `URRRunFlowSubsystem::ResolveRunActors`, `URRStressSubsystem`); first-living-miner lookup 7 times (`ARREnemy::FindTarget`, `RRCheats` x2, flow field, run flow, stress x2). A 4-level run that swaps grids must touch all of them.
- F7 "reachable floor cells around a point" scan written 4 times: `URRRunFlowSubsystem::FindConsoleCell` (56), `FindShuttleCell` (49), `ARREnemySpawner::GetSpawnCandidates` (42), `URRStressSubsystem::SpawnStressEnemies`. The shooter's reposition search would be the fifth.

## Relations
- [[code-review-m1]] — parent index.
- [[code-review-m1-systems]] — findings F8 to F12: data, damage numbers, grid, objectives, shared pawn plumbing.
- [[code-review-m1-plan]] — what to do about each finding.
- [[sys-enemies]] — robot base as built.
- [[sys-projectiles]] — projectile behavior as built.
