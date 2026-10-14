---
id: task-166-02-debris-code
description: "C++ side of the debris: pooled debris Niagara components, random choice of 2 or 3 pieces, last-hit direction, launch when the robot disappears."
parent: task-166-robot-debris-pieces
kind: task
status: done
---
# Robot debris: C++ selection, direction and pool

- goal: C++ side of the debris: pooled debris Niagara components, random choice of 2 or 3 pieces, last-hit direction, launch when the robot disappears.
- agent: cpp
- depends_on: task-166-01-robot-debris-pieces
- inputs: enemy-death (vault/reference/gdd/enemies/enemy-death.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), sys-enemies-chaser (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-chaser.md), sys-actor-pool (vault/reference/tech/systems/world/sys-actor-pool.md), sys-damage (vault/reference/tech/systems/core/sys-damage.md), sys-projectiles (vault/reference/tech/systems/combat/sys-projectiles.md), sys-explosions (vault/reference/tech/systems/combat/sys-explosions.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-tests-enemies (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-enemies.md)
- expected_output: In Source/RogueRigs/: (1) ARREnemy records the direction of the last hit (projectile velocity, or radial from the explosion centre; none = random horizontal); (2) a struct FRRDebrisPiece {Group (Torso, ArmL, ArmR, LegL, LegR), Kind (Whole, Upper, Lower), MeshIndex, Offset (cm, robot space)} and on ARREnemy an EditDefaultsOnly UNiagaraSystem* DebrisSystem plus TArray<FRRDebrisPiece> DebrisPieces (null or empty = M1 death); (3) at the end of the death shrink, before XP and release, choose 2 or 3 meshes (equal odds): draw distinct groups, each limb whole or cut (50 %), a cut limb gives 2 meshes, never more than 3; (4) spawn through a pool of at most 40 Niagara components (none free = skip), set user parameters User.PieceMeshIndex (int array), User.PieceOffset (vector array, rotated by the actor yaw), User.HitDirection (vector) with UNiagaraDataInterfaceArrayFunctionLibrary, auto-release on finish; (5) the Niagara contract written in sys-enemies-base (death bullet) so the bp agent builds to it; the offsets of the 13 pieces come from Art/Source/chaser-meshy/debris/debris_pieces.json (mesh index = order in that report) and are written as the BP_Chaser defaults by the bp agent later, not in C++
- done_when: Build passes with the editor closed; the existing enemy tests still pass (update only an expectation this change breaks, e.g. ShotHitsMovingRobotAndDeathShowsParts if its parts check changes); the selection rule (2 or 3 meshes, cut limb counts 2) is covered by the code path and stated in the doc; doc updated; committed by path
- editor_state: closed
- token_ceiling: open
- model: opus
- tokens: input 50, cache write 83779, cache read 1475465, output 20694
- runs: 1
- milestone: M3
- result: FRRDebrisPiece/ERRDebrisGroup/Kind + FRRDebrisSelection::Choose; ARREnemy DebrisSystem, DebrisPoolSize 40, DebrisPieces, RecordHitDirection (projectiles, explosions); LaunchDebris at shrink end from the feet, yaw-rotated offsets, world-space emitters; contract User.PieceMeshIndex/PieceOffset/HitDirection in sys-enemies-base; build OK, 15/16 tests (known EyeMesh); Niagara not exercised; commit ae8d609

## Relations
- [[task-166-robot-debris-pieces]] — parent global task.
