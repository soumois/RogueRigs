---
id: task-240-01-number-rise-variation
description: "Each damage number rises with its own random speed (x0.7 to x1.3) and angle (up to 35 degrees left or right of screen up), so consecutive numbers no longer travel in parallel."
parent: task-240-number-rise-variation
kind: task
status: todo
---
# Damage number rise variation (C++ draw)

- goal: Each damage number rises with its own random speed (x0.7 to x1.3) and angle (up to 35 degrees left or right of screen up), so consecutive numbers no longer travel in parallel.
- agent: cpp
- depends_on: none
- inputs: damage-numbers (vault/reference/gdd/weapons/damage-numbers.md), sys-damage-numbers (vault/reference/tech/systems/combat/sys-damage-numbers.md), sys-damage-numbers-niagara (vault/reference/tech/systems/combat/sys-damage-numbers-niagara.md)
- expected_output: Per number (all digits of a number share it) a speed factor in [0.7, 1.3] and a signed angle in [-35, 35] degrees around StackDirection are drawn in C++ and written as new Niagara user parameter array User.DigitVelocities (Niagara Vector Array, one per digit, uu/s world space, built from RiseVelocity); config properties RiseSpeedMin 0.7, RiseSpeedMax 1.3, RiseAngleMax 35; sys-damage-numbers and sys-damage-numbers-niagara updated with the contract. Stacking keeps the mean RiseVelocity as approximation.
- done_when: Digits of one number share one velocity; two numbers of one batch differ; the array has DigitCount entries; build passes; lint clean. The asset is not changed here (subtask 02).
- editor_state: closed
- token_ceiling: 90000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-240-number-rise-variation]] — parent global task.
