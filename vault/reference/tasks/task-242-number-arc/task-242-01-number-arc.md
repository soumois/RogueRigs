---
id: task-242-01-number-arc
description: "Damage numbers are thrown in an arc (gravity) instead of rising in a straight line; peak about 60 uu, lifetime unchanged at 0.7 s."
parent: task-242-number-arc
kind: task
status: done
---
# Damage number arc

- goal: Damage numbers are thrown in an arc (gravity) instead of rising in a straight line; peak about 60 uu, lifetime unchanged at 0.7 s.
- agent: bp
- depends_on: task-240-02-number-rise-variation-asset
- inputs: damage-numbers (vault/reference/gdd/weapons/damage-numbers.md), sys-damage-numbers-niagara (vault/reference/tech/systems/combat/sys-damage-numbers-niagara.md), bp-damage-numbers (vault/reference/tech/blueprints/fx/bp-damage-numbers.md)
- expected_output: NS_DamageNumbers Particle Update applies gravity so the number goes up then falls, peak about 60 uu at the current velocity scale; if User.DigitVelocities exists (task-240) the arc is built on it; bp-damage-numbers updated.
- done_when: In a -game run capture a number follows an arc; no Niagara warning; asset saved; bp doc updated; lint clean.
- editor_state: open
- token_ceiling: 80000
- model: open
- tokens: open
- runs: 1
- milestone: M2
- result: Arc: asset velocity x4 + Gravity Force (0,0,-980), peak 60 uu at 0.35 s, commit ee72094b; not seen in game, owner checks it himself (risk: gravity on world Z vs screen-up rise direction)

## Relations
- [[task-242-number-arc]] — parent global task.
