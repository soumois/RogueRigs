---
id: task-324-01-plus-glyph
description: "Add a plus glyph in cell 10 of the damage digit atlas T_DamageDigitsOutline, digits 0-9 untouched, for the heal numbers."
parent: task-324-heal-numbers
kind: task
status: done
---
# Plus glyph in the damage digit atlas

- goal: T_DamageDigitsOutline holds a "+" glyph in cell 10, in the style of the digits, so C++ can draw "+N" through the existing `User.DigitIndices` (SubImage index = glyph index; no Niagara or material change).
- agent: agent-bp
- depends_on: none
- inputs: bp-damage-numbers (vault/reference/tech/blueprints/fx/bp-damage-numbers.md), sys-damage-numbers-niagara (vault/reference/tech/systems/combat/sys-damage-numbers-niagara.md)
- expected_output: /Game/RogueRigs/FX/T_DamageDigitsOutline reimported in place: 1024x64 RGBA, 16 cells of 64 px; cell 10 = "+" drawn on the same 5x7 pixel font grid (6 px per font pixel, soft edges), RGB white inside, 3 px black outline, alpha = glyph plus outline; cells 0-9 and 11-15 kept as they are (start from the existing texture pixels, the original generator script was not kept); bp-damage-numbers `assets` line updated (cell 10 = "+", task-324-01); one commit `task-324-01-plus-glyph: <summary>`.
- done_when: texture still 1024x64 with 16 cells; cell 10 shows a centred "+" with the digits' stroke width and outline; cells 0-9 pixel-identical to before; NS_DamageNumbers and M_DamageNumber unchanged; `RogueRigs.DamageNumbers.NiagaraIntegration` passes in the editor; bp-damage-numbers updated; lint clean; committed by path.
- editor_state: open
- token_ceiling: 40000
- model: sonnet
- tokens: input 76, cache write 83588, cache read 1957096, output 26773
- usage: turns 38, context start 9k peak 84k, resumes 0, 6 min, units 0.43 M
- runs: 1
- milestone: M2
- started: 2026-10-27T12:38:55Z
- waiting_for: none
- result: Plus glyph in cell 10 of T_DamageDigitsOutline, cells 0-9 pixel-identical, NiagaraIntegration test passes; not checked visually (render checked in subtask 02 capture).

## Relations
- [[task-324-heal-numbers]] — parent global task.
