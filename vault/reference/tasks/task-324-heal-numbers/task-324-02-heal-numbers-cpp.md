---
id: task-324-02-heal-numbers-cpp
description: "C++ heal number path: green +N on the miner on each heal pickup, through the damage number subsystem; docs and a -game capture."
parent: task-324-heal-numbers
kind: task
status: todo
---
# Heal numbers in C++

- goal: when a heal pickup heals the miner, `URRDamageNumberSubsystem` shows one green "+N" above him, with the values decided in [[task-324-heal-numbers]] `gdd_check` and `request` (owner answers Q1 to Q6 override the defaults listed there).
- agent: agent-cpp
- depends_on: task-324-01-plus-glyph
- inputs: task-324-heal-numbers (vault/reference/tasks/task-324-heal-numbers.md, decided values), sys-damage-numbers (vault/reference/tech/systems/combat/sys-damage-numbers.md), sys-damage-numbers-niagara (vault/reference/tech/systems/combat/sys-damage-numbers-niagara.md), bp-damage-numbers (vault/reference/tech/blueprints/fx/bp-damage-numbers.md), sys-pickups (vault/reference/tech/systems/world/sys-pickups.md), sys-player (vault/reference/tech/systems/core/sys-player.md), sys-gas-core (vault/reference/tech/systems/core/sys-gas-core.md), damage-numbers (vault/reference/gdd/weapons/damage-numbers.md), heal-ore (vault/reference/gdd/terrain/heal-ore.md), color-code (vault/reference/gdd/art-audio/color-code.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md).
- expected_output: a heal entry point on `URRDamageNumberSubsystem` (e.g. `AddHeal(Location, Amount)`) whose layout puts glyph index 10 ("+", cell 10 of the atlas) before the digits, colour from a new config `HealColor` (decided value), never critical, stacking footprint counting the "+"; called from the code that applies a heal pickup to the miner, at the miner capsule top + 40 uu, with the decided amount, nothing when the amount is 0; robot damage numbers unchanged. Docs updated: sys-damage-numbers (heal path, `HealColor`, entry point), sys-damage-numbers-niagara (`User.DigitIndices` 0-10, 10 = "+"), sys-pickups or sys-player where the heal is applied. No new automation test; only fix existing test expectations this change breaks. One commit `task-324-02-heal-numbers-cpp: <summary>`.
- done_when: build passes; `RogueRigs.DamageNumbers.*` and pickup tests pass; a `-game` run (`rr.Debug.Shots`, see agent-cpp) captures a green "+N" above the miner after a heal pickup below full health, compared with a capture before the change, both paths reported for the owner; damage numbers on robots still white / light orange; docs updated; lint clean; committed by path.
- editor_state: closed
- token_ceiling: 80000
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-324-heal-numbers]] — parent global task.
