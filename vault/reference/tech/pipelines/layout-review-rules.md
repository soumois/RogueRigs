---
id: layout-review-rules
description: "Level-Design review rules for generated 240 x 240 layouts: seed pick among passing seeds, dead-zone and straight-lane checks, pocket distance."
parent: pipelines-index
kind: note
---
# Layout review rules

Refined by the Level-Design agent while writing the run levels ([[task-205-level-layouts-240]]). They complement the generator acceptance of [[layout-generator-recipe]] and the validator of [[level-layout-schema]], which do not see them. Checked by hand in the review for now; not yet automated in `arena_gen_fine.js`.

## Seed pick (from arena_l2, task-205-02)
- Never keep the first seed that passes. Run 6 to 10 passing seeds, print each one downscaled (1 char per 4 × 4 cells) with the min and max open-floor fraction over 24 × 24 windows holding no void, and pick on that.
- Dead zone: reject a seed with a void-free 24 × 24 window under about 0.04 open (seeds 2207 and 3000 passed acceptance with a solid rock mass of about 96 cells). Reference spread: arena_m1_fine 0.11 to 0.99, arena_l2 0.10 to 0.92.
- Too open: a window above about 0.95 open makes combat density uneven; flag it.
- Straight lane: flag a corridor 3 to 10 cells wide running straight for more than about 30 cells (arena_l2 keeps one of about 36 cells on its left side, accepted).
- Hidden pockets: a pocket farther than `max_far` (15 cells) from the network fails (seed 3517: 37); the 1 to 3 rock cell gap alone is not enough.
- Reach window 0.65 to 0.86 is narrow: seeds fail at 0.868 to 0.874; widen to about 0.88 only if passing seeds are scarce.
- Barrel count is drawn per seed (14 to 18), not set by the params.

## Relations
- [[pipelines-index]] — parent index.
- [[layout-generator-recipe]] — the generator these rules review.
- [[level-layout-schema]] — the validator bounds.
- [[agent-level-design]] — the agent that applies them.
