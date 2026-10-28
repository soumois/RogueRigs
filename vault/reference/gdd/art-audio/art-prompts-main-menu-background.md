---
id: art-prompts-main-menu-background
description: "Main menu background: verbatim Kie.ai image-to-image prompt (key art, rigs, robots references), run files, self-review of 3 variants. Open to redo or correct the image."
parent: art-prompts
kind: note
---
# Main menu background — prompt and review

Look decided in [[main-menu]]. Made by [[task-337-01-main-menu-background]]. Run 20261027-172530 (3 variants, 18 credits), files `Art/Source/main-menu-background/20261027-172530-main-menu-background-{1,2,3}.png`, log `.json`, prompt `Art/Source/main-menu-background/prompts/01-scene.txt`. References, in order: `keyart.jpg`, `rigs-lineup.jpg`, `enemies-lineup.jpg` (all in `vault/assets/`). Not validated; nothing copied to `vault/assets/`. Delivery: 16:9, 1K, no text, no UI.

## 1. Scene (verbatim)
```
Same art style as all previous images. Image 1 is the key art scene (style, rock, crystals, void and planet reference). Image 2 is the miner and his drone design (patched white and teal suit with rust, small floating drone). Image 3 is the corporate robot design (yellow chasers with one big red eye and a small yellow logo plate).
Make a new main menu background: flat-shaded low-poly 3D game screenshot, 16:9, no textures, visible facets. Three-quarter camera at about 35 degrees above the ground, lower than the key art camera, so the horizon of the void is visible.
Setting: the edge of the arena asteroid. In the right half of the frame, the miner stands at the faceted cliff edge seen from behind and slightly from the side, facing the void, with his small floating drone hovering at his shoulder. Beyond the cliff, a vast void with an indigo, violet and magenta nebula, a dense starfield, a few floating grey-violet asteroids and a large distant cratered planet as in image 1.
Terrain: light grey rock blocks with a violet tint, with glowing violet Duskite crystals growing in the rocks near the cliff and behind.
Behind the miner, in the middle distance, a few clean yellow corporate chaser robots with glowing red eyes and small yellow logo plates climb over the rocks toward him, small in frame, none firing.
Composition: the left third of the image is calmer, darker and emptier (dark rock and deep space shading, no characters, no bright details), to hold a menu column. The miner, drone and robots stay in the centre and right.
Strong directional light with lifted shadows so everything stays readable. The player side uses white, teal and rust only. No UI, no text, no logo.
```

## Self-review (art-style-guide)
- Variant 1: pass. Miner white, teal, rust (A6), drone with thrusters and orange eye, three chasers yellow with red eye and logo plate (A5), violet crystals, nebula, planet, asteroids, left third dark and empty, no text or UI. Miner is large in frame, not small.
- Variant 2: gap. Drone has a drill bit under it, the validated drone has a mining laser emitter (art-prompts-rigs 3); a pink patch on the miner's boot (rust expected). Rest as variant 1.
- Variant 3: gap. Same drill bit on the drone. Best crystals and nebula, chasers read clearly. Rest as variant 1.
- All three: ground is a rocky ridge rather than the key art plates, and the chasers climb a slope on the left, not a flat edge; the cliff is implied by the rock edge. Camera lower than A12 on purpose ([[main-menu]]).
- No correction pass made; the owner picks first.

## Relations
- [[art-prompts]] — parent: method and locked phrases.
- [[main-menu]] — the decided look.
- [[task-337-01-main-menu-background]] — the subtask that made it.
- [[art-prompts-arena-final]] — key art prompt the style comes from.
- [[art-prompts-rigs]] — miner and drone designs.
- [[art-prompts-robots]] — chaser design.
