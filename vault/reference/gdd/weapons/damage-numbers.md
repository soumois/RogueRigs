---
id: damage-numbers
description: "Floating damage numbers: colors for normal and critical hits, merging window, toggle. Open for combat feedback or UI work."
parent: weapons-index
kind: spec
status: provisional
---
# Damage numbers

- Normal hits: white. Critical hits: 1.6 × larger, gold, linear colour (1, 0.55, 0.08), with a short pop, like DRGS (owner 2026-10-05; colour confirmed by the owner after handover-2026-10-06).
- In a crowd, a new number stacks vertically above an open number whose footprint it would overlap (confirmed by the owner after handover-2026-10-06).
- Hits on the same robot within a short window add up into one number.
- The player can turn them off.
- Rendered with Niagara at horde scale (digit atlas), fed by the damage pipeline ([[sys-damage]]).
- Merging window duration: 0.25 s (provisional, owner 2026-10-05).
- Milestone: M2, built now (owner 2026-10-05). M1 has a debug text version on the test target dummy (owner, 2026-10-01), extended to every enemy (owner, 2026-10-02); not pooled, so not for horde scale.

- No merging (owner 2026-10-08, after the DRGS video): one number per hit; each number starts at a random point of a 30 uu disc around the hit (45 uu on the heavy) so repeated hits on one robot do not start from the same spot; the vertical stacking against overlap is kept. Replaces the 0.25 s merge window above.

## Relations
- [[weapons-index]] — parent index.
