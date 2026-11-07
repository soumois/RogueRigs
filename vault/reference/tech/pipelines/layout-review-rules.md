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
- Barrel count is drawn per seed within `barrels.min` to `barrels.max`, not picked in the review: 17 to 22 since task-491 (owner 2026-11-06, provisional); the committed levels were drawn in 14 to 18. Heal veins: 14 of 2 × 2 (5 in pockets, 9 in walls, brief bound `heal_ore` [56, 56]) since task-491; the committed levels keep 12 (48 cells).

## Refinements from arena_l3 (task-205-03)
- Yield: only 3 to 4 of 28 passing seeds keep every 24 × 24 window between 0.05 and 0.95; review 20 or more passing seeds, not 6 to 10. Most rejects are a fully open window (0.97 to 1.00) or a solid one (0 to 0.03).
- Scan: stride 2, floor, barrel and spawn count as open; results differ by about 0.01 from the first arena_l2 scan.
- Straight lane: an exact repeated interval is too strict on jagged walls; check with a tolerance of ±1 cell, or visually on the downscaled preview.
## Refinements from arena_l4 (task-205-04)
- Yield with default walk params: 1 of 30 passing seeds kept every window in 0.05 to 0.95 (arena_l3: 3 to 4 of 28); 20 seeds is a minimum, not a guarantee of a choice.
- Outline variety: arena_l2 and arena_l3 share the outline harmonics; a different `noise_seed` and harmonic phases give a different shape (arena_l4).
- Straight lane over about 30 cells: flagged, not rejecting (arena_l2 36 cells and arena_l4 44 cells accepted); whether it becomes a hard rule is the owner's call.
- Layout JSON omits floor cells: a scan defaults missing cells to floor.

## Automation
- Doing the scan by hand per seed is the main cost; candidate for automation in the generator acceptance (not done).
- Owner 2026-11-07 (provisional, [[task-495-generator-window-repair]]): the C++ generator enforces every rule of this node, so no agent is needed at runtime: window rule (every void-free 24 × 24 window, stride 2, open fraction 0.05 to 0.95) by a repair pass; straight lane becomes a hard rule (no corridor 3 to 10 cells wide straight over 30 cells, ±1 tolerance), repaired where feasible; a hidden pocket farther than 15 cells (max_far) from the network fails. Each is also a seed-acceptance check. The Node generator is unchanged and the committed maps are not regenerated; the level-design review stays a dev-time check.

## Relations
- [[pipelines-index]] — parent index.
- [[layout-generator-recipe]] — the generator these rules review.
- [[level-layout-schema]] — the validator bounds.
- [[agent-level-design]] — the agent that applies them.
