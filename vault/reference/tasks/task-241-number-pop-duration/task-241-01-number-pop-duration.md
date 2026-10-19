---
id: task-241-01-number-pop-duration
description: "The pop of the damage numbers lasts 0.25 s instead of 0.1 s."
parent: task-241-number-pop-duration
kind: task
status: in-progress
---
# Damage number pop 0.25 s

- goal: The pop of the damage numbers lasts 0.25 s instead of 0.1 s.
- agent: bp
- depends_on: none
- inputs: damage-numbers (vault/reference/gdd/weapons/damage-numbers.md), sys-damage-numbers-niagara (vault/reference/tech/systems/combat/sys-damage-numbers-niagara.md), bp-damage-numbers (vault/reference/tech/blueprints/fx/bp-damage-numbers.md)
- expected_output: NS_DamageNumbers pop duration 0.25 s (scale curve and crit flash stretched to it); bp-damage-numbers and damage-numbers updated (provisional, owner 2026-10-19).
- done_when: The asset pop module reads 0.25 s; no Niagara warning; asset saved; bp doc updated; lint clean.
- editor_state: open
- token_ceiling: 80000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-241-number-pop-duration]] — parent global task.
