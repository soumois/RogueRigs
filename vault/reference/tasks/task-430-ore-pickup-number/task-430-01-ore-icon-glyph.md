---
id: task-430-01-ore-icon-glyph
description: "Bp: add the Duskite icon glyph in cell 11 of the damage digit atlas"
parent: task-430-ore-pickup-number
kind: task
status: todo
---
# Ore pickup number on the miner: bp

- goal: cell 11 of T_DamageDigitsOutline holds the Duskite icon (from `Art/UI/Icons/T_Icon_Ore.png`, area-resampled to fit the 64 px cell with a 3 px black outline like the digits, own colours kept, alpha = icon plus outline), cells 0-10 and 12-15 pixel-identical; the asset reimported with the same settings and M_DamageNumber still referencing it.
- agent: bp
- depends_on: none
- inputs: bp-damage-numbers (vault/reference/tech/blueprints/fx/bp-damage-numbers.md), wbp-ore-counter (vault/reference/tech/blueprints/ui/wbp-ore-counter.md), task-324-01-plus-glyph (vault/reference/tasks/task-324-heal-numbers/task-324-01-plus-glyph.md), agent-bp (vault/reference/agents/agent-bp.md), `Art/UI/Icons/T_Icon_Ore.png`, `Art/Source/damage-digits/T_DamageDigitsOutline.png`
- expected_output: updated `Art/Source/damage-digits/T_DamageDigitsOutline.png` (keep), previous atlas kept as `Art/Source/damage-digits/T_DamageDigitsOutline_before_task-430-01.png` (keep), reimported /Game/RogueRigs/FX/T_DamageDigitsOutline; throwaway script in `Saved/Scratch/`; change summary (cell placement, outline, import settings) for the doc subtask; no doc node edited.
- done_when: cell 11 shows the icon with outline and transparent background (pixel check of corners alpha 0); a pixel diff shows cells 0-10 and 12-15 unchanged; the asset is saved and M_DamageNumber compiles; `RogueRigs.DamageNumbers.NiagaraIntegration` passes in the editor; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-430-ore-pickup-number]] — parent global task.
