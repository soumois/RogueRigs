---
id: art-prompts-card-back
description: "Level-up card back (face-down, four-point star emblem), neutral grey candidate: verbatim prompt, reference, delivery to 1000 x 1440, review. Five tier versions (grey, green, blue, purple, yellow) delivered."
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

## Step 3 — five rarity versions (task-243-03)
Owner 2026-10-19: the A6-corrected image is the validated base; the color-code deviation (dark gunmetal and navy field dominant) is kept on purpose. Grey = the base itself (no generation). Green, blue, purple, yellow: one correction each, --ref `Art/Source/card-back/20261019-150141-card-back-1.png`, 3:4, 1 variant, 6 credits each (24 total), run one at a time. Tier colours from [[wbp-upgrade-choice]] SetCard (grey 0.55,0.55,0.55; green 0.2/0.8/0.25; blue 0.2/0.45/1; purple 0.65/0.25/0.9; yellow 1/0.85/0.1). Changed: star, its glow, the recessed inner line, the two patch plates that were teal; off-white plates, rails and geometry unchanged.
- green: run `20261019-150515-card-back`, prompt `prompts/03-back-green.txt`
- blue: run `20261019-150637-card-back`, `prompts/03-back-blue.txt`
- purple: run `20261019-150758-card-back`, `prompts/03-back-purple.txt`
- yellow: run `20261019-150919-card-back`, `prompts/03-back-yellow.txt`
Each image is `Art/Source/card-back/<run>-card-back-1.png`.

### Prompts (verbatim)
- green: Keep this exact image: same frame geometry, same dark gunmetal rails with rust chips and rivets, same bolts, same off-white corner blocks and off-white hazard-stripe bands, same off-white patch plates, same dark navy panel and recessed inner line shape, same centred faceted four-point star shape and size, same proportions, same flat magenta FF00FF background inside the margin. Only change: the tier colour becomes vivid green (RGB 0.2, 0.8, 0.25). The star emblem becomes faceted metal in that vivid green (RGB 0.2, 0.8, 0.25) with a soft vivid green (RGB 0.2, 0.8, 0.25) glow, the thin recessed inner line near the frame becomes that vivid green (RGB 0.2, 0.8, 0.25), and the two teal patch plates on the side rails become that vivid green (RGB 0.2, 0.8, 0.25) (same positions). Everything else stays identical, including the off-white plates. No text, no icons.
- blue: Keep this exact image: same frame geometry, same dark gunmetal rails with rust chips and rivets, same bolts, same off-white corner blocks and off-white hazard-stripe bands, same off-white patch plates, same dark navy panel and recessed inner line shape, same centred faceted four-point star shape and size, same proportions, same flat magenta FF00FF background inside the margin. Only change: the tier colour becomes vivid blue (RGB 0.2, 0.45, 1.0). The star emblem becomes faceted metal in that vivid blue (RGB 0.2, 0.45, 1.0) with a soft vivid blue (RGB 0.2, 0.45, 1.0) glow, the thin recessed inner line near the frame becomes that vivid blue (RGB 0.2, 0.45, 1.0), and the two teal patch plates on the side rails become that vivid blue (RGB 0.2, 0.45, 1.0) (same positions). Everything else stays identical, including the off-white plates. No text, no icons.
- purple: Keep this exact image: same frame geometry, same dark gunmetal rails with rust chips and rivets, same bolts, same off-white corner blocks and off-white hazard-stripe bands, same off-white patch plates, same dark navy panel and recessed inner line shape, same centred faceted four-point star shape and size, same proportions, same flat magenta FF00FF background inside the margin. Only change: the tier colour becomes vivid purple (RGB 0.65, 0.25, 0.9). The star emblem becomes faceted metal in that vivid purple (RGB 0.65, 0.25, 0.9) with a soft vivid purple (RGB 0.65, 0.25, 0.9) glow, the thin recessed inner line near the frame becomes that vivid purple (RGB 0.65, 0.25, 0.9), and the two teal patch plates on the side rails become that vivid purple (RGB 0.65, 0.25, 0.9) (same positions). Everything else stays identical, including the off-white plates. No text, no icons.
- yellow: Keep this exact image: same frame geometry, same dark gunmetal rails with rust chips and rivets, same bolts, same off-white corner blocks and off-white hazard-stripe bands, same off-white patch plates, same dark navy panel and recessed inner line shape, same centred faceted four-point star shape and size, same proportions, same flat magenta FF00FF background inside the margin. Only change: the tier colour becomes vivid yellow gold (RGB 1.0, 0.85, 0.1). The star emblem becomes faceted metal in that vivid yellow gold (RGB 1.0, 0.85, 0.1) with a soft vivid yellow gold (RGB 1.0, 0.85, 0.1) glow, the thin recessed inner line near the frame becomes that vivid yellow gold (RGB 1.0, 0.85, 0.1), and the two teal patch plates on the side rails become that vivid yellow gold (RGB 1.0, 0.85, 0.1) (same positions). Everything else stays identical, including the off-white plates. No text, no icons.

### Delivery
Node script keys the magenta by flood fill from the border (so the purple tier is not eaten), removes the 2 px tinted fringe, crops to the frame box (947-948 x 1354, ratio 0.700), resizes with premultiplied area averaging to 1000 x 1440 (distortion under 1 percent), transparent outside the frame. Output `Art/Source/card-back/final/card-back-<tier>.png` for grey, green, blue, purple, yellow. Magenta fringe check on edge pixels: 0 on all five. Not copied to vault/assets (owner validates first).

### Review (A rules)
- Same family on all five: identical geometry, rails, bolts, off-white corners and bands, navy panel, star shape; only star, glow, inner line and the two former teal plates change. A1, A2, A14: pass; A6 white plates kept.
- green: pass. blue: pass. purple: pass (star slightly less saturated and more translucent than the others, within tier). yellow: pass.
- Gap: grey keeps the teal patch plates of the base (the tier colour there is teal, not grey); teal and the four tier colours remain distinguishable by the star and line. Glow is a soft painted halo inside the panel, not a separate layer.

## Relations
- [[art-prompts]] — parent: method and locked phrases.
- [[wbp-upgrade-choice]] — card size 1000 x 1440 textures and frame family.
- [[rarities]] — the five tiers the next subtask colours.
