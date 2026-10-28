---
id: art-prompts-heal-pickup
description: "Kie.ai prompt and self-review for the 3-up sheet of red heal pickup crystal variants (task-336-01): verbatim prompt, run, image path, gaps."
parent: art-prompts
kind: note
---
# Heal pickup crystal variants: prompt chain (task-336-01)

Look decided by the owner 2026-10-28 (Q4, Q5, [[heal-ore]]): compact cluster of 3 to 5 chunky faceted pure red points, no rock, same crystal as the heal deposits, 3 variants sharing size, proportions and count, differing by arrangement and angles. Generated through Kie.ai ([[kie-image-workflow]]), image-to-image, reference `vault/assets/heal-deposit-sheet.png` ([[art-prompts-ore-deposit]]), aspect 16:9, 1K.

## Step 01 prompt (verbatim, `Art/Source/heal-pickup/prompts/01-sheet.txt`)
```
Same art style, same camera and same red crystal as the attached image (heal deposit sheet), but a new asset: a 3 x 1 sheet of three separate isolated heal pickup crystal clusters, side by side, wide landscape format, same scale and same camera. Each cluster is a compact group of exactly four chunky faceted glowing saturated red crystal points with large flat facets, joined at one small common base, and NO rock, NO stone pad, NO floor, NO tall spire: a small loose pickup, each cluster about as wide as it is tall. All three clusters share the same overall size, the same proportions, the same crystal count (four) and the same crystal sizes; only the arrangement and lean angles of the four points differ slightly from one to the next (left: points fanning outward; middle: points leaning to the right; right: points leaning to the left with one pointing forward). The three silhouettes stay near identical. Three-quarter top view from about 55 degrees, plain light grey background, no cast shadows, no text, no labels, no pebbles. Visible facets, no smoothing, soft red inner glow, crystals clearly brighter than the background, pure red only: no orange, no pink, no yellow, no violet. Clean hand-painted low-poly game look, readable silhouette at small size, usable as a Blender modelling reference.
```

## Run
- Run log `Art/Source/heal-pickup/20261027-163753-heal-pickup.json`, image `Art/Source/heal-pickup/20261027-163753-heal-pickup-1.png` (1 variant, 6 credits, no correction pass needed). Delivery: 3-up sheet, plain light grey background, three-quarter top view; not copied to `vault/assets/` (owner validates first).

## Self-review
- A1 pass: large flat facets visible, no smoothing. A12/A16 pass: three-quarter top view, plain light grey background. A14 pass: no text, UI, pebbles, floor or shadows.
- [[color-code]] and [[readability-rules]] pass: pure saturated red with inner glow, no orange, pink, yellow or violet; no rock, so not a red copy of the violet ore chunk.
- Brief pass: each cluster shows 4 crystals, same size and proportions, arrangements differ (fan, lean right, lean left with a front point).
- Gaps: the three silhouettes are close but not identical in height (right cluster slightly taller); the glow is a soft facet highlight with no halo; the base is a single merged crystal mass, to be modelled flat-bottomed in Blender.

## Relations
- [[art-prompts]] — parent: method and locked style phrases.
- [[art-prompts-ore-deposit]] — the heal deposit sheet used as image reference.
- [[heal-ore]] — decided pickup variant look.
- [[task-336-01-heal-crystal-variants]] — the subtask that made this sheet.
