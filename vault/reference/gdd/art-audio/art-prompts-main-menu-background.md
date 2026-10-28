---
id: art-prompts-main-menu-background
description: "Main menu background: verbatim Kie.ai image-to-image prompt (key art, rigs, robots references), run files, self-review of 3 variants. Open to redo or correct the image."
parent: art-prompts
kind: note
---
# Main menu background — prompt and review

Look decided in [[main-menu]]. Made by [[task-337-01-main-menu-background]]. Run 20261027-172530 (3 variants, 18 credits), files `Art/Source/main-menu-background/20261027-172530-main-menu-background-{1,2,3}.png`, log `.json`, prompt `Art/Source/main-menu-background/prompts/01-scene.txt`. References, in order: `keyart.jpg`, `rigs-lineup.jpg`, `enemies-lineup.jpg` (all in `vault/assets/`). Owner validated variant 1 (2026-10-28), copied to `vault/assets/main-menu-background.png`. Delivery: 16:9, 1K, no text, no UI.

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

## Self-review of the first run (art-style-guide)
- Variant 1: pass. Miner white, teal, rust (A6), drone with thrusters and orange eye, three chasers yellow with red eye and logo plate (A5), violet crystals, nebula, planet, asteroids, left third dark and empty, no text or UI. Miner is large in frame, not small.
- Variant 2: gap. Drone has a drill bit under it, the validated drone has a mining laser emitter (art-prompts-rigs 3); a pink patch on the miner's boot (rust expected). Rest as variant 1.
- Variant 3: gap. Same drill bit on the drone. Best crystals and nebula, chasers read clearly. Rest as variant 1.
- All three: ground is a rocky ridge rather than the key art plates, and the chasers climb a slope on the left, not a flat edge; the cliff is implied by the rock edge. Camera lower than A12 on purpose ([[main-menu]]).
- No correction pass made; the owner picks first.

## Decision
- 2026-10-28: the owner keeps the first version, variant 1 of run 20261027-172530, as `vault/assets/main-menu-background.png` (his first pick of variant 3 was a coordinator mistake, corrected the same day). Correction pass 1 (made on variant 3) is unused; no second pass.
- Known open fails of the style-guide review of variant 1, accepted by the owner "for now": A3 squat crouched chasers, A13 no dark floor (everything on light rock), cracks on rock tops (readability), A5 worn chasers, a lit slope entering the left third, drone with thruster flames instead of the rigs-lineup cone. Redo with the prompt of section 1 plus these fixes if wanted later.
- Total cost 24 credits over 2 runs.

## 2. Correction pass 1 (verbatim, made on variant 3, unused)
Owner picked variant 3 of run 20261027-172530. Run 20261027-173523 (1 variant, 6 credits), file `Art/Source/main-menu-background/20261027-173523-main-menu-background-1.png`, log `.json`, prompt `Art/Source/main-menu-background/prompts/02-correction-1.txt`. References, in order: variant 3 (image to correct), `rigs-lineup.jpg`. Made on variant 3, not kept.
```
Keep this exact image: same art style, same flat-shaded low-poly look, same camera angle, same 16:9 composition, same dark left third with no characters, same three clean yellow chaser robots (tall thin upright bodies, glowing red eyes, small yellow logo plates) climbing on the left, same glowing violet crystals, same indigo violet magenta nebula, same starfield, same floating grey-violet asteroids, same large cratered planet on the right, same miner suit (patched white and teal with rust) and same small floating drone with thrusters. No text, no UI. Image 1 is the image to correct. Image 2 is the validated miner and drone design (rigs-lineup), used only for the drone laser emitter.
Only change these five things:
1. The miner now stands on a ledge of dark violet-magenta floor made of flat plates at the cliff edge; this floor is clearly darker than the grey-violet rocks around it.
2. The rock tops are smooth with large flat facets, no cracks and no fine surface detail.
3. The cliff edge is more explicit: a clear sheer drop from the ledge straight down into the void, with the edge line visible and nothing below it but space.
4. The miner is smaller in the frame, about 70 percent of his current size, still in the right half, with the drone beside him.
5. The drone has the mining laser emitter exactly as in image 2 (the validated drone design) instead of the drill bit under it.
Change nothing else.
```

### Self-review (correction 1)
- Pass: miner smaller in frame; drone now has a laser emitter with a beam (as the validated drone), no drill bit; rock tops smoother with large flat facets, no cracks; cliff edge reads as a sheer drop into the void; chasers (tall thin, yellow, red eyes), crystals, nebula, planet, asteroids, dark empty left third, miner white, teal, rust (A6), no text or UI all kept.
- Gap: the whole rock mass went darker and violet, so the ledge is not clearly a separate darker floor of flat plates (A13): the miner stands on a rock top of the same violet tone, not on distinct plates. The drone emits a visible beam, the validated drone reference shows only the emitter. Left third is less distinct from the rock since everything darkened.

## Relations
- [[art-prompts]] — parent: method and locked phrases.
- [[main-menu]] — the decided look.
- [[task-337-01-main-menu-background]] — the subtask that made it.
- [[art-prompts-arena-final]] — key art prompt the style comes from.
- [[art-prompts-rigs]] — miner and drone designs.
- [[art-prompts-robots]] — chaser design.
