---
id: task-430-ore-pickup-number
description: "Global task: floating Duskite icon plus "+1" above the miner when he collects a Duskite chunk, rendered like the damage numbers."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Ore pickup number on the miner

- request: "j'aimerais que lorsqu'on ramasse un minerai (seulement le mauve pour l'instant), on ait un texte comme les dégâts mais avec l'icône du minéral ramassé et +1 à côté" (owner, 2026-11-04). Restated: on collecting a violet ore chunk (Duskite, the only ore type; the red heal pickups already show a green +N), show a floating text in the damage-number style made of the collected ore icon followed by "+1".
- goal: each time the miner collects a Duskite chunk (Ore pickup), a floating number "<Duskite icon> +N" appears above him with the look and motion of a damage number (digit atlas, arc, pop, fade).
- acceptance: in a `-game` run, collecting a Duskite chunk shows one floating number above the miner: the Duskite icon (own colours, readable, not overlapping the "+") then "+1" in the colour decided in Q2; one number per chunk (Q5), stacked when several arrive together; heal pickups still show a green "+N" with no icon; robot damage numbers unchanged (white, critical light orange); XP pickups show nothing; the damage-number toggle hides it (Q7); capture sent to the owner; build and existing `RogueRigs.DamageNumbers.*` and pickup tests pass; committed.
- gdd_check: read [[ore-blocks]] (Duskite = violet crystals, the only ore type; chunk value `OreTypes.csv` row `Duskite` column `ChunkValue`, 1 today; on screen the ore is called "Duskite"), [[color-code]] (Duskite violet; heal ore and heal pickups red; heal number green), [[damage-numbers]] (look, size by value, arc, pop, stacking, no merge, toggle; heal numbers line = the precedent), tech [[sys-damage-numbers]] (`AddHeal`, `AppendDigits`, `PlusGlyph` 10, `DigitAdvance` 0.62), [[sys-damage-numbers-niagara]] (glyph index = atlas cell), [[bp-damage-numbers]] (T_DamageDigitsOutline 16 cells of 64 px, cells 0-10 used, 11-15 free; material emissive = ParticleColor x texture RGB, so a coloured glyph drawn with white particle colour keeps its colours), [[sys-pickups]] (Ore pickups add `OreValue` on collection; heal number call in `ARRPickup::ApplyHeal`), [[wbp-ore-counter]] (HUD ore icon T_Icon_Ore, source `Art/UI/Icons/T_Icon_Ore.png`, owner-chosen). "Seulement le mauve" read as: Ore pickups only (Duskite); heal (red) keeps its green number, XP nothing. Conflicts: none. Open values: none in the gdd; the new values are the Q1-Q8 defaults, written provisional into [[damage-numbers]] (ore numbers line) and [[color-code]] (ore number row) on the owner answer. Milestone: M2 feature already built, no scope issue. Not a representative-slice case (same path as task-324 heal numbers).
- confirmed_by_owner: no (plan and questions handed back 2026-11-04)
- milestone: M2
- next_step: task-430-01-ore-icon-glyph
- started: open
- waiting_for: owner answers to Q1-Q8 and plan confirmation
- result: open

## Questions
Asked 2026-11-04, each with a default; open until the owner answers.
- Q1 Value shown: "+N" with N = the ore value of the chunk (`ChunkValue`, 1 today, so "+1"), so it follows the table if the value changes. Default: yes.
- Q2 Colour of "+1": white like a normal damage number; the icon carries the violet. Alternative: Duskite violet text. Default: white.
- Q3 Icon: the HUD ore icon (T_Icon_Ore, the crystal you chose) shrunk into a free 64 px cell (cell 11) of the damage digit atlas, own colours, with the same 3 px black outline as the digits; placed left of "+1", the whole centred. Default: yes.
- Q4 Size and motion: same rules as a damage number of that value (value 1 = the smallest size, 0.8 x), same arc, pop and fade, never critical. Alternative: a fixed larger size. Default: same rules.
- Q5 Several chunks collected together (a deposit gives 4): one number per chunk, no merge, vertical stacking like damage and heal numbers. Alternative: merge into "+4". Default: one per chunk.
- Q6 Position: top of the miner capsule + 40 uu with the 30 uu random disc, like the heal number. Alternative: where the chunk is absorbed. Default: on the miner.
- Q7 The "show damage numbers" setting also hides ore numbers. Default: yes.
- Q8 Scope: Duskite chunks only; heal pickups keep their green "+N" without icon, XP pickups show nothing; a future ore type would get its own icon cell later. Default: yes.

## Subtasks
1. task-430-01-ore-icon-glyph (vault/reference/tasks/task-430-ore-pickup-number/task-430-01-ore-icon-glyph.md) — bp: add the Duskite icon glyph in cell 11 of the damage digit atlas
2. task-430-02-ore-number-cpp (vault/reference/tasks/task-430-ore-pickup-number/task-430-02-ore-number-cpp.md) — cpp: ore number path in the damage number subsystem, fired on Ore pickup collection
3. task-430-03-ore-number-capture (vault/reference/tasks/task-430-ore-pickup-number/task-430-03-ore-number-capture.md) — capture: `-game` capture of the ore number on Duskite collection
4. task-430-04-ore-number-doc (vault/reference/tasks/task-430-ore-pickup-number/task-430-04-ore-number-doc.md) — doc: document the ore number in the damage number, Niagara contract, atlas and pickup docs

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-430-01-ore-icon-glyph]] — step 1, bp.
- [[task-430-02-ore-number-cpp]] — step 2, cpp.
- [[task-430-03-ore-number-capture]] — step 3, capture.
- [[task-430-04-ore-number-doc]] — step 4, doc.
- [[task-324-heal-numbers]] — the heal number precedent this task copies.
