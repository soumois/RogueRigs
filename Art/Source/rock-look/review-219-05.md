# Review 219-05: correction of image C (rock and ore look)

Reviewer: style-guide agent, 2026-10-17. Judged on the images alone, against `art-style-guide` (A rules), `color-code`, `readability-rules`, `visual-style` and the `corrections` checklist of task-219-04.
Reference: `20261016-231332-rock-look-1.png` (image C, owner's direction: one continuous slab, rounded organic outline).

Checklist items used below:
- K1 camera 75–80 deg from horizontal (A12)
- K2 flat colour per facet, no gradient or grain on the top (A1)
- K3 cracks irregular, crossing zones without seams
- K4 light cracks thick enough to read at game distance
- K5 heavy damage = plates tilted up
- K6 each deposit = four crystal clusters in a 2 x 2 square
- K7 no orange in the heal vein
- K8 corridors 3 cells wide, outline more cut up
- K9 keep C's rounded slab, lighter rim, faceted sides

---

## Image 1: `20261017-131704-rock-look-1.png`

Board of 7 separate slabs: intact, light cracks (x2), heavy cracks, ore, heal, one small round stub.

| Check | Verdict | What I see | Correction for the next prompt |
|---|---|---|---|
| A1 / K2 | fail | The top faces carry a soft mottled grain and slight shading gradient instead of one flat colour per facet. | "each top facet is one flat untextured colour, no grain, no noise, no gradient" |
| A12 / K1 | fail | Camera is about 50–55 deg from horizontal; side walls take a large share of the frame. | "camera looking almost straight down, 75–80 degrees from horizontal, side walls only a thin band" |
| A13 | pass | Floor is clean dark slate with large subtle facets, no pebbles. | — |
| A14 | pass | No UI, text or scattered rubble; the round stub is about one cell, reads as a small block. | — |
| color-code: breakable rock | fail | The intact slab, the ore slab and the heal slab show no crack at all, while breakable rock is "light grey tinted violet, visible cracks". | "even the intact state shows a few short visible cracks" (or ask the owner to confirm that the intact state may be crack-free) |
| color-code: ore | pass | Glowing violet crystal clusters on the same grey-violet base. | — |
| color-code: heal ore | pass | Pure red clusters with red glow. | — |
| readability: brightness | pass | Rock clearly lighter than the floor; ore glows. | — |
| readability: enemy colours | pass | No acid green or steel blue on terrain. | — |
| K3 | fail | Each damage state sits on its own separate slab, so cracks never cross zones, and the cracks form a regular cell (Voronoi) network; image C's continuous slab is lost. | "one continuous slab whose damage grows from intact to heavy across it, cracks long, irregular and branching, not a cell pattern" |
| K4 | pass | Light cracks are dark, branching and long enough to read. | — |
| K5 | fail | Heavy-damage plates are flat pieces separated by gaps; none is tilted up. | "in the heavy zone several plates are tilted up at an angle, one edge raised above the surface" |
| K6 | pass | Ore and heal deposits are each four clusters in a 2 x 2 square. | — |
| K7 | pass | No orange in the heal vein. | — |
| K8 | fail | Gaps between slabs are narrow (under one cluster width), not 3-cell corridors; the separate blobs do not form a cut-up mass. | "slab masses separated by floor corridors three cells wide, outline with deep notches and lobes" |
| K9 | pass | Rounded organic outlines, lighter rim band, faceted sides kept. | — |

Verdict image 1: **fail** (6 fails: A1, A12, color-code, K3, K5, K8).

---

## Image 2: `20261017-131704-rock-look-2.png`

Board of 5 masses: big ore slab with damage growing to the right, intact-to-heavy slab top right, heal slab with damage on the right, small intact slab bottom left, tiny stub.

| Check | Verdict | What I see | Correction for the next prompt |
|---|---|---|---|
| A1 / K2 | fail | The top faces still carry a faint mottled grain and soft gradients. | "each top facet is one flat untextured colour, no grain, no noise, no gradient" |
| A12 / K1 | fail | Camera is about 50–55 deg from horizontal, same as image 1. | "camera looking almost straight down, 75–80 degrees from horizontal, side walls only a thin band" |
| A13 | pass | Clean faceted floor, no pebbles. | — |
| A14 | fail | The tiny round stub between the masses (well under one cell) reads as an isolated piece of rubble on the floor. | "no isolated small rock pieces on the floor; every rock is at least one cell" |
| color-code: breakable rock | fail | The bottom-left slab and the intact zones show no crack, while breakable rock is "visible cracks". | "even the intact state shows a few short visible cracks" (or ask the owner to confirm that the intact state may be crack-free) |
| color-code: ore | pass | Glowing violet clusters on the grey-violet base. | — |
| color-code: heal ore | pass | Pure red clusters with red glow. | — |
| readability: brightness | pass | Rock lighter than the floor; ore glows. | — |
| readability: enemy colours | pass | No enemy colour on terrain. | — |
| K3 | fail | Cracks cross the zones without seams (good), but they still form a regular polygon cell network, not irregular branching cracks. | "cracks long, irregular and branching, of uneven length, not a honeycomb or cell pattern" |
| K4 | fail | The lightest cracks (middle of the top-right slab, left edge of the cracked zones) are hairlines that will vanish at game distance. | "light cracks are thick dark grooves clearly visible from far away" |
| K5 | pass | In the heavy zones several plates have raised, tilted edges over dark gaps. | — |
| K6 | pass | Ore and heal deposits are each four clusters in a 2 x 2 square. | — |
| K7 | pass | No orange in the heal vein. | — |
| K8 | pass | Masses are separated by wide floor corridors; the outlines are lobed with notches. | — |
| K9 | pass | Continuous rounded slab, lighter rim band, faceted sides kept, as in image C. | — |

Verdict image 2: **fail** (6 fails: A1, A12, A14, color-code, K3, K4).

---

## Better base

**Image 2.** It keeps image C's continuous slab with damage growing across it without seams, has tilted heavy plates and wide corridors with a cut-up outline. Image 1 breaks C into separate one-state slabs, which loses the owner's chosen direction. Remaining work on image 2: steeper camera (A12), flat untextured top facets (A1), thicker and less cell-like light cracks (K3, K4), drop the tiny stub (A14), and a decision by the owner on whether intact rock must show cracks (color-code).
