# Review 219-02: rock and ore look boards

Reviewer: style-guide agent, 2026-10-17. Rules: A1–A18 ([[art-style-guide]]), [[color-code]], [[readability-rules]]; brief checks from task-219-02 (three states readable at game camera, 2 x 2 deposit and vein, clean floor).
Not applicable to a terrain board: A3–A11, A15–A18. A2 passes on all four (rigid rock, no cloth or organic forms).

## Track A-1 — `20261016-231225-rock-look-1.png` (per-cell blocks)

| Rule | Verdict | What I see |
|---|---|---|
| A1 | fail | Cube edges are rounded and bevelled, tops carry a soft painted grain and gradient; facets are not flat colour per material. |
| A12 | fail | Camera is about 45 degrees; the front side faces of the cubes are as visible as the tops. |
| A13 | pass | Dark floor with large faint facets, no pebbles. |
| A14 | pass | No UI, no text, no scattered rubble (chips stay attached to the block corners). |
| color-code | fail | A column of three cells (centre-right, top) is a darker neutral grey with no violet tint: a block type that exists nowhere in the code. |
| readability: brightness | pass | Rock is clearly lighter than the floor. |
| readability: ore glows | pass | Duskite violet and heal red both glow and outshine the rock. |
| readability: red = heal static | pass | Red is only on the static vein crystals. |
| brief: three states | pass | Intact, thin-crack and broken-plate cells separate clearly, even at thumbnail size. |
| brief: 2 x 2 ore | pass | One cluster per cell, 2 x 2 squares read at a glance. |

Corrections:
- A1: "flat-shaded low-poly, every facet one flat colour, hard sharp edges with no bevel or rounding, no painted texture, no surface grain, no gradient on top faces".
- A12: "camera looking almost straight down, about 75 to 80 degrees from horizontal; top faces fill the view, side faces show only as a thin strip".
- color-code: "every plain rock cell is the same light grey tinted violet; no darker, neutral grey or empty cells inside the mass".

## Track A-2 — `20261016-231225-rock-look-2.png` (per-cell blocks)

| Rule | Verdict | What I see |
|---|---|---|
| A1 | fail | Rounded bevelled cube edges, soft painted shading and small notched chips on the tops. |
| A12 | fail | Camera about 40–45 degrees; side faces large, long cast shadow to the left. |
| A13 | pass | Clean dark faceted floor. |
| A14 | pass | No UI, text or loose rubble. |
| color-code | pass | Violet-grey rock, violet Duskite, red vein; no crossed colours. |
| readability: brightness | pass | Rock lighter than floor. |
| readability: ore glows | pass | Both glow; crystals are small but the glow pool marks the 2 x 2 area. |
| readability: red = heal static | pass | |
| brief: three states | fail | Light cracks are hair-thin, low-contrast lines; at game camera distance they will merge with intact cells. Heavy plates (tilted) read well. |
| brief: 2 x 2 ore | pass | |

Corrections:
- A1 and A12: same wording as A-1.
- Three states: "light damage: three to five thick, dark crack lines per cell, clearly visible from far away; heavy damage: top split into four to six plates, each visibly tilted".
- Optional: "crystal clusters fill most of the cell top" (crystals are small here).

## Track B-1 — `20261016-231332-rock-look-1.png` (continuous slab)

| Rule | Verdict | What I see |
|---|---|---|
| A1 | fail | Sides are good faceted low-poly, but the top is a smooth painted surface with mottled grain and soft gradients. |
| A12 | fail | Camera about 45 degrees; thick side band visible all around. |
| A13 | pass | Clean dark faceted floor, no pebbles. |
| A14 | pass | No UI, no text; small crystal shards lie around the clusters, on the rock top (not floor rubble, but watch it). |
| color-code | pass | Rock top reads more neutral grey than violet-tinted, but within "light grey tinted violet". |
| readability: brightness | pass | |
| readability: ore glows | pass | |
| readability: red = heal static | pass | |
| brief: three states | fail | Heavy damage is a flat honeycomb of hexagonal tiles with dark gaps, not tilted plates; light cracks are also a regular honeycomb, so the two states read as tiling patterns, and the intact zone is fenced by a seam like a separate tile. |
| brief: 2 x 2 ore | fail | One round cluster per deposit; no 2 x 2 footprint is readable. |

Corrections:
- A1: "the top face is flat-shaded low-poly too: large flat facets, one flat colour each, no painted texture or grain".
- A12: same wording as A-1.
- Three states: "irregular, uneven crack lines, no hexagons or honeycomb; heavy damage: the top is broken into irregular plates, each visibly tilted and raised at one edge; no seams or rims separating the zones, one continuous top".
- 2 x 2: "the Duskite deposit covers a square patch of 2 by 2 cells: four crystal clusters in a square, rock visible between them; the heal vein the same in red".

## Track B-2 — `20261016-231332-rock-look-2.png` (continuous slab)

| Rule | Verdict | What I see |
|---|---|---|
| A1 | fail | Faceted sides pass; the top is smooth with soft gradients and painted shading. |
| A12 | fail | Camera about 45 degrees. |
| A13 | pass | |
| A14 | pass | |
| color-code | pass | One red crystal has an orange inner hotspot; a detail, the vein stays red (colour principle). |
| readability: brightness | pass | |
| readability: ore glows | pass | |
| readability: red = heal static | pass | |
| brief: three states | pass (weak) | Heavy plates are raised and slightly tilted and read well; light cracks are thin and branching, readable here but at risk at game distance; the intact zone is again fenced by a seam. |
| brief: 2 x 2 ore | fail | One cluster per deposit; the 2 x 2 size does not read. |

Corrections:
- A1, A12, 2 x 2: same wording as B-1.
- Three states: "light cracks thicker and darker; no seams between zones, one continuous top"; optional: "crystals pure red, no orange highlights" (orange means "explodes").

## Cross-check
All four: no crossed faction colours, no acid green or steel blue, no UI or text, clean floor; all four fail A1 (smooth or painted top surfaces, bevelled edges) and A12 (about 45 degrees instead of steep near-vertical). Neither the camera nor the shading can be judged as final from these boards; a redo of the chosen track with the A1 and A12 wording is needed before any 3D work.

## Ranking
1. A-1: strongest separation of the three states, clear 2 x 2 ore; fix A1, A12 and the darker grey column.
2. B-2: best heavy damage (raised tilted plates) and closest to the owner reference slab; fix A1, A12, 2 x 2 footprint, zone seams.
3. A-2: connected mass and good plates, but light cracks too faint and crystals small.
4. B-1: heavy and light damage read as honeycomb tiling, plates not tilted, no 2 x 2 footprint.
