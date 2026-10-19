---
id: task-240-02-number-rise-variation-asset
description: "NS_DamageNumbers moves each digit with the per-number velocity written by C++ in User.DigitVelocities."
parent: task-240-number-rise-variation
kind: task
status: done
---
# Damage number rise variation (Niagara asset)

- goal: NS_DamageNumbers reads User.DigitVelocities (Niagara Vector Array, one entry per digit, same index as the other arrays) and moves each digit with it, replacing the fixed rise of 85.7 uu/s straight up.
- agent: bp
- depends_on: task-240-01-number-rise-variation
- inputs: damage-numbers (vault/reference/gdd/weapons/damage-numbers.md), sys-damage-numbers-niagara (vault/reference/tech/systems/combat/sys-damage-numbers-niagara.md), bp-damage-numbers (vault/reference/tech/blueprints/fx/bp-damage-numbers.md)
- expected_output: NS_DamageNumbers saved with the new user parameter read in Particle Spawn (index Particles.DNIdx) and applied as the particle velocity (rise unchanged otherwise: lifetime 0.7 s, fade, pop); bp-damage-numbers updated.
- done_when: the asset exposes User.DigitVelocities and uses it; no Niagara warning in the editor log; asset saved; bp doc updated; lint clean.
- editor_state: open
- token_ceiling: 80000
- model: open
- tokens: open
- runs: 1
- milestone: M2
- result: NS_DamageNumbers Particle Spawn reads User.DigitVelocities (SelectVectorFromArray, index DNIdx), compile clean, commit 580beca4; not run in PIE, LogNiagara data-interface message at add time untraced

## Relations
- [[task-240-number-rise-variation]] — parent global task.
