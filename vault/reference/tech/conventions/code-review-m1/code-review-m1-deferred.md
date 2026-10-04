---
id: code-review-m1-deferred
description: "Refactors from the M1 review not done in task-020-02: gain, risk, hours, Blueprint impact and the trigger that should schedule each one."
parent: code-review-m1
kind: doc
---
# Deferred refactors

Continues the ranking of [[code-review-m1-plan]] (R1 to R7 are the 020-02 scope).

| # | Item | Gain | Risk | h | Blueprint impact / trigger |
|---|---|---|---|---|---|
| R8 | F2/D4: kamikaze `ArmRange`, `FuseTime` to CSV `AttackRange`, `WindUp` (150, 0.5) | medium | medium | 1 | Done in task-021-01: properties removed, values unchanged ([[sys-enemies-kamikaze]]). |
| R9 | Split `RRIntegrationTests.cpp` per namespace, `RRWeaponTests.cpp` in two | low | low | 1 | Test-only; any time. |
| R10 | F12: shared `ARRCombatCharacter` base for miner and robots | medium | medium | 4 | Reparents BP_Miner and every robot BP; subobject names must stay. After M2 enemies exist. |
| R11 | F10: split grid layout parsing into `FRRGridLayout` + `ApplyLayout` | high for generator | low | 2 | No BP; do in the grid-generator task. |
| R12 | F10: per-block-type table instead of 25 members | medium | high | 4 | BP_ArenaGrid overrides `RockMesh` and the others: values must be re-entered; only if block types grow. |
| R13 | F11: `ARRObjective` base (zone, drone link, progress) | medium | medium | 3 | Delegate types change (`ARRConsole*`): HUD widget rebinds. When the second objective is scheduled. |
| R14 | D1: delete `Variant_TwinStick` C++ and content, drop its include paths | medium | medium | 2 | bp agent deletes `Content/Variant_TwinStick` first; own task. |
| R15 | D2, D3: remove `URRGameplayAbility`, `GuaranteedWeaponClasses` | low | low | 0.5 | Tests need a shotgun loadout helper first. |
| R16 | F3: rename the two movement components | low | medium | 1 | Reflected class and subobject `EnemyMovement`: CoreRedirects needed. Better: clarify comments only. |

## Relations
- [[code-review-m1]] — parent index.
- [[code-review-m1-plan]] — the recommended scope and the ranking these items continue.
