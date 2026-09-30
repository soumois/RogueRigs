---
id: damage-numbers
description: "Floating damage numbers: colors for normal and critical hits, merging window, toggle. Open for combat feedback or UI work."
parent: weapons-index
kind: spec
status: provisional
---
# Damage numbers

- Normal hits: white. Critical hits: larger, white-gold.
- Hits on the same robot within a short window add up into one number.
- The player can turn them off.
- Rendered with Niagara at horde scale (digit atlas), fed by the damage pipeline ([[sys-damage]]).
- Merging window duration: open.
- Milestone: M2 (critical-hit pop in M3).

## Relations
- [[weapons-index]] — parent index.
