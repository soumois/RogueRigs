---
id: task-239-01-number-size-by-damage
description: "The size of a damage number grows with the damage of the hit, continuous from x0.8 to x1.4; a crit keeps its x1.6 on top."
parent: task-239-number-size-by-damage
kind: task
status: in-progress
---
# Damage number size by damage

- goal: The size of a damage number grows with the damage of the hit, continuous from x0.8 to x1.4; a crit keeps its x1.6 on top.
- agent: cpp
- depends_on: none
- inputs: damage-numbers (vault/reference/gdd/weapons/damage-numbers.md), sys-damage-numbers (vault/reference/tech/systems/combat/sys-damage-numbers.md), sys-damage-numbers-niagara (vault/reference/tech/systems/combat/sys-damage-numbers-niagara.md)
- expected_output: AddHit/push computes Size from the hit value: lerp(0.8, 1.4, clamp(Value / DamageForMaxSize, 0, 1)) x NumberSize, then x CriticalScale on a crit; new config properties SizeMin 0.8, SizeMax 1.4, DamageForMaxSize = twice the base damage of the starting weapon (read from the weapon stats table; report the value found); docs sys-damage-numbers and damage-numbers updated (provisional, owner 2026-10-19); existing tests updated if broken.
- done_when: A 1-damage hit and a hit at DamageForMaxSize give sizes x0.8 and x1.4 of NumberSize in the digit array; crit multiplies by 1.6; build passes; lint clean. Stacking footprint uses the new size.
- editor_state: closed
- token_ceiling: 90000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-239-number-size-by-damage]] — parent global task.
