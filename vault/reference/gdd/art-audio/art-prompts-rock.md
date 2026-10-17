---
id: art-prompts-rock
description: "Rock and ore look proposals (task-219-01): per-cell blocks versus continuous slab, verbatim prompts, images, review gaps. Track C correction (task-219-04) added."
parent: art-prompts
kind: note
---
# Rock and ore look: proposals

Owner request 2026-10-17: draw plain rock intact, lightly cracked and heavily fractured, a Duskite deposit and a heal vein, from the game camera. Cracked means "already damaged, digs faster" ([[rock-look-reference]], third-party, never attached). Colours: [[color-code]]. Both prompts are text-to-image (no reference), 16:9, 1K, 2 variants each (24 credits in total, about 6 per image).

## Track A: per-cell blocks
Prompt `Art/Source/rock-look/prompts/01-track-a-cells.txt`; run `Art/Source/rock-look/20261016-231225-rock-look.json`; images `...-1.png`, `...-2.png`.
Chunky 67 uu cubes with gap lines; left to right intact, light cracks, tilted plates; chipped corners on the open edge; 2 x 2 violet deposit and 2 x 2 red vein of crystal clusters, one per cell.

## Track B: continuous slab
Prompt `Art/Source/rock-look/prompts/02-track-b-slab.txt`; run `Art/Source/rock-look/20261016-231332-rock-look.json`; images `...-1.png`, `...-2.png`.
One flat top with a rounded irregular outline, lighter rim, dark faceted sides; same three states as patches; one big violet and one big red crystal cluster.

## Correction of image C (task-219-04)
Owner kept image C (track B, `20261016-231332-rock-look-1.png`) and rejected A. Prompt `Art/Source/rock-look/prompts/04-c-fix.txt` (verbatim, image-to-image with C as `--ref`, 16:9, 1K, 2 variants, 12 credits); run `Art/Source/rock-look/20261017-131704-rock-look.json`; images `...-1.png`, `...-2.png`.
- Camera: both still about 50 to 60 degrees, not 75 to 80; sides remain a thick band (gap, partly fixed only).
- Flat facets: tops are mostly flat colour with faint grain; sides faceted (partial pass).
- Zones: image 2 blends intact into light cracks into plates with no seam (pass); image 1 keeps separate slabs per state (gap). Cracks still read as honeycomb/cell patterns in places (gap); plates tilted and readable (pass).
- Ore: four violet clusters and four red clusters in 2 x 2 with rock between, red has no orange (pass).
- Scale and outline: 5 to 6 masses with floor corridors, rounded cut-up outline, lighter rim, clean floor (pass); corridors are about 3 cells only by eye, masses are not clearly many cells across.

## Delivery format
Board on a clean dark floor, no characters, no text; verbatim prompts are the files above.

## Review gaps (both tracks)
- Camera is about 45 degrees, not steep near-vertical; sides of cubes and slab show more than in game.
- Shading is soft and slightly painterly: facets and flat colour per material (A1) are only partly kept.
- Track A: the mass is flat and wide with no wall depth; plates read well; ore reads as one crystal tuft per cell, cells stay separate.
- Track B: the slab is small (about one lobe, not many cells); ore is one cluster per deposit, so the 2 x 2 size is not readable; cracks and plates read best here.
- Floor clean, no pebbles (A13, A14 pass); ore brighter than rock (pass); violet-tinted grey rock (pass).

## Relations
- [[art-prompts]] — parent: method and locked phrases.
- [[rock-look-reference]] — the owner reference these proposals answer.
- [[ore-blocks]] — the deposit rules the ore blocks follow.
