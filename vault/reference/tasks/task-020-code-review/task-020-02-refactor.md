---
id: task-020-02-refactor
description: "Refactor R1, R2, R4, R5 from code-review-m1-plan (robot base helpers, reachable-cell query, world lookups, projectile filter and weapon aim); no behavior change."
parent: task-020-code-review
kind: task
status: done
---
# Refactor

- goal: the shared robot, query and projectile plumbing the shooter needs, with identical behavior.
- agent: cpp
- depends_on: task-020-01-review
- inputs: [[code-review-m1-plan]] items R1, R2, R4, R5 (020-01 split the recommended scope in two passes), [[cpp-standard]], [[sys-automation-tests]].
- expected_output: refactors in priority order within the token ceiling (stop and list what remains); any rename of a UCLASS, USTRUCT, UENUM, UPROPERTY or UFUNCTION used by Blueprints or assets gets a CoreRedirect in `Config/DefaultEngine.ini`, or is avoided; test count unchanged or higher; system docs updated where structure changed; the review node marks each item done or deferred.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 150000
- model: opus
- tokens: input 134, cache write 191429, cache read 7041553, output 68988
- runs: 2
- milestone: M1
- result: R1 robot helpers in ARREnemy (SetMovementActive, IsTargetWithin, MarkStateStart, GetTimeInState, HasStateLasted, ApplyDamageSpecTo, UpdateTelegraphBlink; ArrivalRangeRatio moved up, same name). R2 URRFlowFieldSubsystem::GatherReachableCells + GatherPawnCells replace 4 scans, same order (54 seeded log lines identical). R4 RRWorld::FindArenaGrid, FindMiner, FindLivingMiner replace 10 loops. R5 ARRProjectile CanHit (HitActorClass), bDamagesTerrain, static SpawnProjectile; ARRWeapon aim and shot origin virtual; test Weapons.ProjectileHitFilter. 101 → 102/102, build 0, no CoreRedirect needed. sys-enemies split into index + base, movement, chaser, kamikaze; new sys-world-queries; sys-flow-field, sys-projectiles, sys-weapons updated. Commits 6fafeb3, 027a33d, bdd99d5, a19ddf0, 608cbbf.

## Relations
- [[task-020-code-review]] — parent global task.
