---
id: task-025-02-bp-numbers
description: "Niagara system rendering digits from an atlas at horde scale: rise and fade, crits larger white-gold with a short pop."
parent: task-025-damage-numbers
kind: task
status: done
---
# Damage numbers Niagara

- goal: numbers look like DRGS.
- agent: bp
- depends_on: task-025-01-numbers-pipeline
- inputs: [[damage-numbers]], task-025-01 result, [[unreal-mcp]], [[sys-cheats]], [[cpp-bp-split]], [[color-code]], [[readability-rules]], [[drgs-analysis]]
- expected_output: NS_DamageNumbers using a digit atlas texture (generated in engine or a simple font atlas asset), sprite per digit, rise 60 uu and fade over 0.7 s, crit scale 1.6× with a 0.1 s pop and white-gold color; bound to the subsystem; doc new bp-damage-numbers. PIE: numbers on hits, crits bigger, toggle works.
- done_when: PIE check through the cheat property route with no log error; Blueprint docs updated; lint clean.
- editor_state: open
- token_ceiling: 110000
- model: sonnet
- tokens: input 150, cache write 153555, cache read 7173595, output 52470
- runs: 2
- milestone: M2
- result: NS_DamageNumbers (one CPU world-space emitter, bounds ±30000, NewCount × 5 digit particles per frame, each reads its number by ExecutionIndex via Select-from-array, HLSL digit extraction, layout along world Y, unused slots killed; life 0.7 s, rise 60 uu, fade last 45 %, pop +15 %/+35 % with white flash on crits; crit inferred from colour since the bool array select lacks direct index), M_DamageNumber (unlit translucent, NiagaraSprites usage), T_DamageDigits (generated 5x7 digit atlas). PIE: no Niagara parameter warning; rendering NOT seen (no screenshots), checked in 025-03. MCP driven over HTTP (tools not exposed to the agent). Docs bp-damage-numbers. Commits 119611c, a9546ec.

## Relations
- [[task-025-damage-numbers]] — parent global task.
