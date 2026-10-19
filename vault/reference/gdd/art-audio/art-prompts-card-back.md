---
id: art-prompts-card-back
description: "Level-up card back (face-down, four-point star emblem), neutral grey candidate: verbatim prompt, reference, delivery to 1000 x 1440, review. Open to make the tier colours."
parent: art-prompts
kind: note
---
# Card back — neutral grey candidate (task-243-01)

Face-down level-up card for the deal and flip ([[wbp-upgrade-choice]]). Same family as the stat card frame; no text, no weapon icon. Tier colours (grey, green, blue, purple, yellow, [[rarities]]) come in the next subtask; owner validation open.

## Run
- Reference (`--ref`): `Art/Source/card-frames/20261014-141713-card-frames-1.png` (validated stat frame).
- Prompt file: `Art/Source/card-back/prompts/01-back-grey.txt`; run `20261019-135524-card-back`, 3 variants, Kie aspect 3:4, 1K, 6 credits each (18).
- Candidates: `Art/Source/card-back/20261019-135524-card-back-1.png` to `-3.png` (1086 x 1448, flat magenta FF00FF outside the frame, as the frame source).
- Best: variant 2 (clean frame, star well lit and centred, frame box 948 x 1350 = ratio 0.702, nearest 25:36 = 0.694). Variant 1: frame touches the left and right margin and its star is smaller; variant 3: good, star smaller and thinner.

## Prompt (verbatim)
Same art style as the attached image (the card frame): flat-shaded low-poly game UI card frame, visible facets, chunky dark gunmetal bolted strips, big octagonal corner blocks with large faceted bolts, hazard-stripe end pieces, patch plates with small rivets, rust chips and wear, thin inlay line along the inner side of the frame. Draw the BACK of a face-down playing card in the same frame design and proportions (portrait card, ratio 25:36, frame fully visible with a margin of flat magenta FF00FF around it on all four sides; the background outside the frame is flat magenta FF00FF). Neutral grey version: replace every teal accent (corner blocks, hazard stripes, inlay line, patch plates) by neutral cool grey steel, a lighter grey than the gunmetal strips, no colour at all. Inside the frame the opening is filled by a solid dark panel (#15171c to #1c1f26) with a subtle flat recessed border line close to the frame, and no pattern. In the exact centre of the panel, one single large four-point star (sparkle) emblem, symmetrical, with long thin vertical and horizontal points and short diagonal waist, made of faceted light grey metal with a soft pale grey glow, about 45 percent of the card width. Nothing else: no text, no letters, no numbers, no weapon, no icon, no characters, no UI. Strong directional light with lifted shadows, so everything stays readable.

## Delivery to 1000 x 1440
Kie has no 25:36 (nearest 3:4 = 0.75). Never stretch. Crop the picture to the frame's bounding box (pixel with colour other than magenta; variant 2: about x 70-1018, y 45-1395), which already has ratio about 0.70, then resize to 1000 x 1440 (distortion under 1 percent), or crop the box to exactly 25:36 trimming 3 px of magenta first. Key out the magenta as for the frames.

## Review (A rules)
- A1 faceted metal, bolts, rust: pass on all three. A2 rigid parts only: pass. A14 no text, no UI, no weapon: pass. Neutral: teal replaced by grey steel on corners, hazard stripes, inlay and patches: pass. Emblem a centred four-point star: pass.
- Gap: the grey star on dark panel is plain (no pattern); the inner panel colour is the card panel #15171c family, not a magenta key, so the opening is not transparent as the frame source.

## Step 2 — A6 correction (owner: variant 2 validated with this change)
- Ref: `Art/Source/card-back/20261019-135524-card-back-2.png`; prompt file `prompts/02-back-grey-a6.txt`; run `20261019-150141-card-back`, 1 variant, 3:4, 6 credits; image `Art/Source/card-back/20261019-150141-card-back-1.png`.
- Prompt (verbatim): Keep this exact image: same frame geometry, same dark gunmetal rails with rust chips and rivets, same bolts, same hazard-stripe end pieces, same dark panel and recessed inner line, same centred faceted four-point star with its pale glow, same proportions, same flat magenta FF00FF background inside the margin. Only change: the four corner blocks and the patch plates (and the hazard-stripe light bands) become off-white painted metal (patched white plates with rust chips, like the player-side Rigs), and one or two of the patch plates on the side rails become teal instead of off-white. Everything else stays identical. No text, no icons.
- Review: pass. Corner blocks, hazard bands and patch plates off-white with rust chips; two teal patch plates (right rail upper, left rail lower); rails, rivets, star, geometry and magenta background unchanged (A6 player-side white and teal). Delivery to 1000 x 1440 as above. Tier colours: next subtask.

## Relations
- [[art-prompts]] — parent: method and locked phrases.
- [[wbp-upgrade-choice]] — card size 1000 x 1440 textures and frame family.
- [[rarities]] — the five tiers the next subtask colours.
