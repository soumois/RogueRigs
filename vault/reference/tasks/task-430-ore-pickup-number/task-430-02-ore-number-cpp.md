---
id: task-430-02-ore-number-cpp
description: "Cpp: ore number path in the damage number subsystem, fired on Ore pickup collection"
parent: task-430-ore-pickup-number
kind: task
status: todo
---
# Ore pickup number on the miner: cpp

- goal: add `URRDamageNumberSubsystem::AddOre(Location, Amount, InSpawnRadius = -1)` (or an equivalent icon-prefixed path through `AddPendingNumber`), constant `OreGlyph` = 11: layout icon glyph, then "+" (`PlusGlyph`), then the digits, the whole centred; the icon advance wide enough that the square icon does not overlap the "+" (spacing value provisional, header default); icon glyph colour always white (keeps its texture colours), "+N" colour = config `OreColor` white (Q2); size by value, never critical, same arc, pop, fade and stacking footprint counting the icon cell; ignored when Amount <= 0 or the toggle is off. Called on collection of an Ore pickup in `ARRPickup` with `OreValue` at the miner capsule top + `HealNumberHeight` (40 uu), default spawn disc. Heal and damage paths unchanged.
- agent: cpp
- depends_on: task-430-01-ore-icon-glyph
- inputs: sys-damage-numbers (vault/reference/tech/systems/combat/sys-damage-numbers.md), sys-damage-numbers-niagara (vault/reference/tech/systems/combat/sys-damage-numbers-niagara.md), sys-pickups (vault/reference/tech/systems/world/sys-pickups.md), sys-pickups-lifecycle (vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md), damage-numbers (vault/reference/gdd/weapons/damage-numbers.md), task-324-02-heal-numbers-cpp (vault/reference/tasks/task-324-heal-numbers/task-324-02-heal-numbers-cpp.md), agent-cpp (vault/reference/agents/agent-cpp.md)
- expected_output: C++ changes in `Source/RogueRigs/Damage/RRDamageNumberSubsystem.h/.cpp` and `Source/RogueRigs/Pickups/RRPickup.h/.cpp`; existing tests updated only if this change breaks them; commit `task-430-02-ore-number-cpp: ...`; change summary (API, constants, defaults, call site) for the doc subtask; no doc node edited.
- done_when: Development Editor build succeeds; `RogueRigs.DamageNumbers.*` and `RogueRigs.Pickups.*` automation tests pass; a headless check (log or `GetLastDigits`) shows an Ore collection pushes digits [11, 10, 1] with the icon white and "+1" in `OreColor`; committed by path.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-430-ore-pickup-number]] — parent global task.
