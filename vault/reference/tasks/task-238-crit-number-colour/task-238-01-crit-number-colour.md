---
id: task-238-01-crit-number-colour
description: "The critical damage number is a lighter orange, linear colour (1, 0.7, 0.35), instead of the current gold (1, 0.55, 0.08)."
parent: task-238-crit-number-colour
kind: task
status: done
---
# Critical damage number colour

- goal: The critical damage number is a lighter orange, linear colour (1, 0.7, 0.35), instead of the current gold (1, 0.55, 0.08).
- agent: cpp
- depends_on: none
- inputs: damage-numbers (vault/reference/gdd/weapons/damage-numbers.md), sys-damage-numbers (vault/reference/tech/systems/combat/sys-damage-numbers.md), sys-damage-numbers-niagara (vault/reference/tech/systems/combat/sys-damage-numbers-niagara.md)
- expected_output: CriticalColor default in RRDamageNumberSubsystem.h and DefaultGame.ini (if set) changed; gdd node damage-numbers and sys-damage-numbers updated with the new value (provisional, owner 2026-10-19); existing tests that expect the old colour updated.
- done_when: The code and the ini give (1, 0.7, 0.35) for a critical number; the two vault nodes say so; build passes; lint clean.
- editor_state: closed
- token_ceiling: 60000
- model: opus
- tokens: input 20, cache write 27829, cache read 179872, output 3345
- runs: 1
- milestone: M2
- result: CriticalColor default (1, 0.7, 0.35) in RRDamageNumberSubsystem.h, tests and 2 vault nodes updated, build exit 0, commit 1efa9a70; not seen in game

## Relations
- [[task-238-crit-number-colour]] — parent global task.
