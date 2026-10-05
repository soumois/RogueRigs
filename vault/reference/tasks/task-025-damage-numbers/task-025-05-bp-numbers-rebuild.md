---
id: task-025-05-bp-numbers-rebuild
description: "Rebuild NS_DamageNumbers on the per-digit contract: infinite loop, spawn DigitCount particles when DigitBatchId changes, SubUV frame from DigitIndices."
parent: task-025-damage-numbers
kind: task
status: in-progress
---
# Rebuild the damage number Niagara

- goal: digits render correctly and continuously.
- agent: bp
- depends_on: task-025-04-per-digit-contract
- inputs: [[sys-damage-numbers]], [[bp-damage-numbers]], [[stress-damage-numbers]], [[unreal-mcp]]
- expected_output: NS_DamageNumbers: emitter loop Infinite and system never completes; spawn DigitCount particles when User.DigitBatchId differs from the last seen value (or the documented Data Channel read); per particle read the per-digit arrays (index from the spawn order: if Engine.ExecutionIndex is 0 in Particle Spawn, use Particles.UniqueID minus the batch start or the module the C++ agent recommends); SubUV frame = digit index on T_DamageDigits; rise 60 uu, fade 0.7 s, crit pop; no HLSL digit extraction left; check with fx.DumpParticleData that indices vary; doc bp-damage-numbers.
- done_when: no Niagara warning in PIE; particle dump shows varied digits; docs updated; lint clean.
- editor_state: open
- token_ceiling: 110000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-025-damage-numbers]] — parent global task.
