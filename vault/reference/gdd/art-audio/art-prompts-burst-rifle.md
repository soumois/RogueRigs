---
id: art-prompts-burst-rifle
description: "ChatGPT four-view turnaround prompt of the burst rifle (default weapon held by the miner) for Meshy, with images to attach. Open for burst rifle art."
parent: art-prompts
kind: note
---
# Burst rifle — turnaround prompt for ChatGPT (four views)

The owner generates the sheet in ChatGPT, as for the miner and the console (owner "ok", 2026-10-30, task-387, Q1 default a). No Kie.ai run, no Meshy credit, no crew-generated image. The sheet goes to Meshy later, in a separate task, once the owner validates it. Prompt file: `Art/Source/burst-rifle/prompts/chatgpt-burst-rifle-turnaround.md`.

## Decisions (owner "ok", 2026-10-30, provisional)
- Q1: the crew writes the prompt and prepares the images; the owner generates the sheet.
- Q2: attach the crop `Art/Source/burst-rifle/reference/rifle-crop-rigs-lineup.png` (1065 x 420, cut from `vault/assets/rigs-lineup.jpg`, 3x nearest-neighbour, whole rifle from stock to muzzle brake; the miner's gloves and arm overlap it, the muzzle flash is left out) then `vault/assets/rigs-lineup.jpg` for the style.
- Q3: keep the rifle the miner holds in `rigs-lineup.jpg`.
- Q4: four views on one sheet, left side, right side, front (muzzle), back (stock), matching the Meshy multi-view slots ([[art-3d-lessons]]).
- Q5: prompt text as drafted, no change.

## Prompt (verbatim)
```
Turnaround sheet for 3D modeling of the default weapon of a stylized space miner, a salvaged two-handed burst rifle, same weapon design as the rifle held by the miner in the attached images: four views on one sheet, left side view (exact profile), right side view (exact profile), front view (looking into the muzzle) and back view (looking at the stock), all four at the same scale, same height and same framing, no perspective distortion, subject centred with a margin, the rifle alone, no hands, no character, no stand, no ground.
Style: stylized flat-shaded low-poly game asset, chunky blocky volumes with large flat facets, facets clearly visible, no smoothing, no textures, clean hard-surface look. Rigid parts only: plates, bolts, rails, no cloth, no organic forms. Slightly oversized, chunky proportions so the rifle reads from a steep top-down camera.
Design: a compact two-handed rifle assembled from salvaged parts: boxy receiver, short shoulder stock, pistol grip and front grip that big armored gloves can hold, a top rail with a small sight, a magazine under the receiver, a short barrel ending in a slotted muzzle brake. A clearly readable front (muzzle) and back (stock).
Colors and wear: patched, mismatched white and teal plates with rust chips, bolts, rivets and welded repairs, dark gunmetal grip, barrel and muzzle, a repaired and improvised salvaged look, same palette as the miner. No red, no yellow, no green, no blue, no violet, no glow.
Background and light: plain white background, even neutral lighting, no shadows, no muzzle flash, no projectiles, no effects, no text, no UI, no logo.
```

## Open values
- Final English name of the weapon: open.
- Mesh size and triangle budget of the rifle: open.
- Budget of Meshy credits: open.
- Miner hand socket and which weapon is shown in hand: open.

## Relations
- [[art-prompts]] — parent: method and locked phrases.
- [[art-prompts-miner]] — the miner turnaround whose design the rifle matches; format model.
- [[art-prompts-rigs]] — source prompt of rigs-lineup.jpg, where the rifle first appears.
- [[art-prompts-console]] — four-view turnaround model.
- [[burst-rifle]] — the weapon spec.
- [[art-3d-lessons]] — Meshy multi-view rule behind the four views.
