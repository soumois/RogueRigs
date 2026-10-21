---
id: art-prompts
description: "Method for generating 2D reference art: locked style phrases, the 'keep this exact image, change only X' loop, and where each verbatim prompt chain lives."
parent: art-audio-index
kind: note
---
# Art prompts — method

Reference images are generated in ChatGPT image generation. The prompts are stored verbatim in the chain nodes below. They are inputs for the art agent and for any new asset in the same style.

## Locked style phrases (reuse first)
- Look: "flat-shaded low-poly", "visible facets", "no textures", "no UI, no text".
- Scene camera: "steep top-down camera (almost vertical, slight perspective), 16:9".
- Light: "strong directional light with lifted shadows so everything stays readable".
- Floor: "large cracked faceted stone plates, painted look, fine crack lines, plate edges catching the light, dark violet-magenta, no pebbles, no rubble" (owner 2026-10-21, provisional, task-273; was "clean, flat, slightly dark, subtle large facets, no pebbles").
- Character sheets: plain light grey background, three-quarter top view. Turnaround for 3D: plain white background, even neutral lighting, no shadows.
- Rigs side: "patched, mismatched white and teal", rust, "repaired, bolted and improvised"; no yellow, green, blue or violet ([[color-code]]).
- Corporate side: "clean industrial", red eyes, small yellow logo plate, rigid mechanical parts only.

## Iteration loop
1. One full prompt describes the sheet or scene. Later prompts open with "Same art style as the previous images".
2. Every correction opens with "Keep this exact image: same X" and changes ONE thing ("Only change ...").
3. Correction prompts assume the previous image is attached. A prompt alone does not reproduce an image.
4. The validated image is the one saved in `vault/assets/`.

## Validated images
- `keyart.jpg` — full arena ([[art-prompts-arena-final]]).
- `enemies-lineup.jpg` — the four corporate robots ([[art-prompts-robots]]).
- `rigs-lineup.jpg` — miner, drone, turret ([[art-prompts-rigs]]).
- `miner-turnaround.jpg` — front, side, back T-pose sheet for 3D ([[art-prompts-rigs]]).
- `shield-turnaround.webp` — the shield chaser riot shield, front, side, back ([[art-prompts-shield]]).
- `drone-turnaround.webp` — the drone, front, side, back, source of the Meshy mesh ([[art-prompts-drone]]).

## Relations
- [[art-audio-index]] — parent index.
- [[art-prompts-arena-draft]] — first arena prompt and the floor cleanup.
- [[art-prompts-arena-final]] — final arena prompt and the miner pose fix.
- [[art-prompts-robots]] — corporate robot sheet and its four corrections.
- [[art-prompts-rigs]] — miner, drone, turret sheet, drone fix, T-pose.
- [[art-prompts-shield]] — shield chaser riot shield: sheet, Meshy views, raw mesh, open texture steps.
- [[art-prompts-robots-meshy]] — shooter, heavy, kamikaze: Meshy sheet and view prompts, not run yet.
- [[art-prompts-drone]] — drone turnaround prompt as used, Meshy mesh files, game meshes.
- [[art-prompts-xp-core]] — four any-angle proposals to replace the big XP chip; choice open.
- [[art-prompts-rock-top-texture]] — tileable painted rock top texture for the slab material: prompt, seam method, review.
- [[art-prompts-floor-texture]] — ChatGPT prompts for the tileable painted floor of cracked faceted plates: chain, seam method, review.
- [[art-prompts-rock]] — rock and ore look proposals: per-cell blocks versus continuous slab; choice open.
- [[art-prompts-card-back]] — level-up card back, neutral grey candidate: prompt, delivery to 1000 x 1440.
- [[art-style-guide]] — the general style rules derived from these prompts.
- [[visual-style]] — the style these prompts produce.
- [[asset-pipeline]] — where the images go next (Meshy, Tripo).
