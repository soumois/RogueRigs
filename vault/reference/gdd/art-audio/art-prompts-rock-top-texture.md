---
id: art-prompts-rock-top-texture
description: "Prompt, run, seam method and review of the tileable painted rock top texture (task-230-03): two candidates, owner pick open."
parent: art-prompts
kind: note
---
# Rock top texture: tileable candidates (task-230-03)

Need: one square tileable painted texture sampled by world position on slab tops ([[rock-render-top-texture]]); light grey tinted violet ([[color-code]]), intact rock smooth, no cracks, no baked light.

## Prompt chain
- Step 01, verbatim file `Art/Source/rock-top-texture/prompts/01-rock-top-texture.txt`; image-to-image, `--ref Art/Source/rock-look/20261017-131704-rock-look-2.png` (tone only), 1:1, 1K, 2 variants, 12 credits; run `Art/Source/rock-top-texture/20261017-153218-rock-top-texture.json`.
- Prompt core: "flat square texture swatch seen from straight above ... light grey tinted violet ... perfectly smooth intact rock, no cracks ... large soft patches of slightly lighter and slightly darker grey-violet, plus a very faint fine grain ... no shadows, no highlights, no directional relief, no vignette".

## Seamless method
`Tools/art/make_tileable.js` (deterministic, no dependency): centre-crop and resample to 1024, remove the per-axis linear tone ramp, shift by half a tile, then cross-fade a soft band (18 percent of the tile) around the old borders toward the unshifted image. Raw files are kept; wrap step versus inner step: candidate 1 4.68 vs 3.97, candidate 2 4.13 vs 3.57 (levels of 255).

## Files
Raw `...-153218-rock-top-texture-<n>.png`, tileable `...-<n>-tileable.png` (1024), 3 x 3 preview `...-<n>-tiled3x3.png` (3072), all in `Art/Source/rock-top-texture/`. Nothing is in `Art/Textures/` until the owner picks.

## Review
- Both: violet-tinted light grey, no cracks, no pebbles, no text, no baked shadow or relief, low contrast; 3 x 3 previews show no seam line.
- Candidate 1: faint scalloped, cloud-edged patches and a visible fine grain; a mild repeating blob motif in the 3 x 3.
- Candidate 2: softer, larger patches, less motif repetition; preferred for the macro sample.
- Gap for both: tone is slightly more saturated violet than the reference rock tops; the half-shift cross-fade blends two different contents along a band (soft ghosting, invisible at game scale).

## Relations
- [[art-prompts]] — parent: method and locked phrases.
- [[art-prompts-rock]] — rock look source the tone comes from.
- [[rock-render-top-texture]] — how the material samples this texture.
