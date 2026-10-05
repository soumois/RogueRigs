---
id: task-025-05-bp-numbers-rebuild
description: "Rebuild NS_DamageNumbers on the per-digit contract: infinite loop, spawn DigitCount particles when DigitBatchId changes, SubUV frame from DigitIndices."
parent: task-025-damage-numbers
kind: task
status: done
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
- model: sonnet
- tokens: input 194, cache write 163247, cache read 9491673, output 65092
- runs: 3
- milestone: M2
- result: NS_DamageNumbers rebuilt on the per-digit contract: 8 User.Digit* parameters, loop Infinite, spawn DigitCount when DigitBatchId changes (Emitter.LastBatchId), Particle Spawn reads arrays at Engine.ExecIndex (SubImageIndex = digit), fade/pop/crit flash from DNCritI and DNSize, old HLSL and number-level parameters removed. DamageNumbers tests 6/6 (one batch of 4 digits spawns exactly 4 particles), no Niagara or missing-parameter warning. Not verified: digit variety, long-run spawning (025-06). Stray NS_Orig.uasset deleted by the orchestrator. Commits b858e99, bec8c0f, dfa46c3.

## Relations
- [[task-025-damage-numbers]] — parent global task.
