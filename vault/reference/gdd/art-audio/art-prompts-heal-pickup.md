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
- Step 01: run log `Art/Source/heal-pickup/20261027-163753-heal-pickup.json`, image `Art/Source/heal-pickup/20261027-163753-heal-pickup-1.png` (1 variant, 6 credits). Style-guide review FAILED it on A16: camera near eye level, undersides and sides visible instead of the tops.
- Step 02 (correction pass 1): run log `Art/Source/heal-pickup/20261027-164005-heal-pickup.json`, image `Art/Source/heal-pickup/20261027-164005-heal-pickup-1.png` (1 variant, 6 credits). References: step 01 image (image 1) and `vault/assets/heal-deposit-sheet.png` (image 2). Delivery: 3-up sheet, plain light grey background, three-quarter top view; not copied to `vault/assets/` (owner validates first).
- Step 03 (correction pass 2): run log `Art/Source/heal-pickup/20261027-165219-heal-pickup.json`, image `Art/Source/heal-pickup/20261027-165219-heal-pickup-1.png` (1 variant, 6 credits). References: pass 1 image (image 1) and `vault/assets/heal-deposit-sheet.png` (image 2). Owner feedback on pass 1: the pickup is thrown out of the broken vein and may roll, so it must read as a freshly broken-off chunk, not a splayed flower. Not copied to `vault/assets/`.

## Step 02 prompt (verbatim, `Art/Source/heal-pickup/prompts/02-correction-camera.txt`)
```
Keep this exact image: same 3 x 1 sheet of three separate isolated heal pickup crystal clusters, same size, same proportions, same four chunky faceted pure red crystal points per cluster, same plain light grey background, same low-poly hand-painted style, no rock, no floor, no text. Change ONE thing: the camera. Raise it to a true three-quarter top view, about 55 degrees looking down, exactly like the camera of the second attached image (heal deposit sheet), so the pointed tops and the upper facets of every crystal point are clearly visible, and the undersides and the lower sides are hidden. The points rise away from the viewer and their tips are seen from above, not from eye level. Give the three clusters a clearly different arrangement of the four points while keeping the same overall size and the same crystal sizes: left cluster, points fanning outward in a wide star; middle cluster, three points leaning to the right with one short upright point; right cluster, points leaning to the left with one pointing toward the camera. Pure red only: no orange, no pink, no yellow, no violet.
```

## Self-review of step 02 (image viewed)
- A16 pass: the camera now looks down on the clusters; the top facets and tips of every point are visible and no undersides show. Caveat: the angle reads closer to 65 to 75 degrees than 55, so the points look splayed flat like a flower instead of rising upright; the deposit sheet is a more oblique view.
- A1 pass: large flat facets, no smoothing. A14 pass: no text, rock, floor, pebbles or cast shadows; plain light grey background.
- [[color-code]] pass: pure saturated red, no orange, pink, yellow or violet. Glow is only facet highlights, no halo.
- Variation pass: the three arrangements now differ clearly (left fan, middle with an upright point and a leaning pair, right leaning with a front point). Each cluster shows four points.
- Gaps: the right cluster is larger than the other two (not the same size); the middle cluster tips are seen at a slightly different angle; the base is a merged mass, to be modelled flat-bottomed in Blender.

## Step 03 prompt (verbatim, `Art/Source/heal-pickup/prompts/03-correction-broken-chunk.txt`)
```
Keep this exact image: same 3 x 1 sheet of three separate isolated heal pickup crystal pieces, same size, same proportions, same pure saturated red color, same chunky large flat facets, same low-poly hand-painted style, same high three-quarter top view camera, same plain light grey background, no rock matrix, no floor, no text, no shadows. Change ONE thing: the shape. Each piece must read as a freshly broken-off chunk of crystal that was thrown out of a broken vein and can rest and roll on the ground: a compact, dense, stocky lump about as wide as it is tall, NOT a flower-like splayed cluster. Build each piece as one solid chunky body with a heavy, wide, solid base, topped by two or three short, stubby, blunt crystal points that stay close together and do not splay outward. The base and lower sides are a rough, jagged, irregular broken fracture surface made of large angular flat chips with sharp broken edges and a few small snapped-off notches, as if the piece was just torn from the vein; the fracture faces are the same pure red, a little darker and duller than the polished crystal faces. The three pieces are near identical in size and proportions and differ only by small changes in the outline and the position of the stubby points. Pure red only: no orange, no pink, no yellow, no violet.
```

## Self-review of step 03 (image viewed)
- Owner feedback pass: each piece is now a compact lump about as wide as tall, with a heavy wide base of jagged angular red chips and 2 main stubby points on top; no flower splay. Reads as a chunk that can rest and roll.
- A1 pass: large flat facets. A14 pass: no text, floor, shadows; plain light grey background. [[color-code]] pass: pure red, no orange or pink; fracture faces slightly darker.
- Three variants near identical in size and proportions, small differences in outline and point positions (left has a small third tip at the front, right has a small tip at the lower left).
- Gaps: the fracture base is a bulky red mass that can read as a red rock matrix, and it is large relative to the crystal points (about half the height); the base is not clearly flat enough to confirm a resting face; points are two large plus one tiny, not 2 to 3 stubby; the camera is slightly lower than the pass 1 top view.

## Relations
- [[art-prompts]] — parent: method and locked style phrases.
- [[art-prompts-ore-deposit]] — the heal deposit sheet used as image reference.
- [[heal-ore]] — decided pickup variant look.
- [[task-336-01-heal-crystal-variants]] — the subtask that made this sheet.
