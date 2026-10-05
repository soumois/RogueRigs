---
id: task-025-02-bp-numbers
description: "Niagara system rendering digits from an atlas at horde scale: rise and fade, crits larger white-gold with a short pop."
parent: task-025-damage-numbers
kind: task
status: in-progress
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
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-025-damage-numbers]] — parent global task.
