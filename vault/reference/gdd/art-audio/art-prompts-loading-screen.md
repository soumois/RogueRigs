---
id: art-prompts-loading-screen
description: "Loading screen illustration: verbatim Kie.ai image-to-image prompts (scene and two corrections), references, runs, credits, style review. Open to redo or correct it."
parent: art-prompts
kind: note
---
# Loading screen illustration — prompts and review

Look decided in [[intermission]] (loading screen, owner 2026-11-06, provisional). Made by [[task-488-01-loading-image]]. Model `gpt-image-2-image-to-image` (Kie.ai, [[kie-image-workflow]]), 16:9, 1K (1672 x 941). Retained image: `Art/Source/loading-screen/20261106-202627-loading-screen-1.png`, copied to `vault/assets/loading-screen-background.png`, imported as `/Game/RogueRigs/UI/T_LoadingBackground` (group UI, no mipmaps, sRGB, never stream). Status: keep. Total 24 credits over 3 runs.

## 1. Scene (verbatim)
Run 20261106-202154 (2 variants, 12 credits), prompt `Art/Source/loading-screen/prompts/01-scene.txt`, log `.json`. References, in order: `vault/assets/main-menu-background.png`, `vault/assets/keyart.jpg`, `vault/assets/rigs-lineup.jpg`, `Art/Source/loading-screen/ref-extractor-three-quarter.png` (render of the re-baked extractor mesh, [[art-prompts-extractor]]). Variant 1 kept as the base; variant 2 temporary, in `Saved/Scratch/task-488/` (busy bottom left, blue drone eyes).
```
Same art style as all previous images. Image 1 is the main menu background (style, light, void, nebula, planet and rock reference). Image 2 is the key art scene (arena asteroid, rock plates, crystals). Image 3 is the miner and his drone design (patched white and teal suit with rust, small floating drone). Image 4 is the run shuttle design: a quadrotor dropship with four ducted rotors, patched cream-white and teal plates with rust, an amber cockpit canopy and four landing legs.
Make a new loading screen illustration: flat-shaded low-poly 3D game screenshot, 16:9, visible facets, no textures.
Subject: the run shuttle from image 4 diving down through the void toward the arena asteroid. The shuttle is in the upper right part of the frame, seen in three-quarter view from above and slightly behind, tilted nose down, its four rotors glowing faintly, a short thin trail behind it. Its side hatch is open: the miner from image 3 crouches in the hatch, holding on, looking down at the asteroid, with his small floating drone hovering beside the hatch.
Below and toward the centre, the arena asteroid: a large floating rock island with flat dark violet-magenta floor plates on top, light lilac-violet faceted rock blocks and glowing violet Duskite crystals, its edges a cliff falling into space.
Background: a vast void with an indigo, violet and magenta nebula, deep ultramarine blue in the depths, a dense white starfield, a few small floating grey-violet asteroids and a large distant cratered planet on the right edge.
Composition: the bottom left quarter of the image is calmer and darker (deep space shading and the dark underside of the asteroid, no characters, no bright details) to hold a text panel. The shuttle, miner and drone stay in the upper half and the right.
Strong directional light with lifted shadows so everything stays readable. The player side uses white, teal and rust only. No robots. No UI, no text, no logo.
```

## 2. Correction 1 (verbatim)
Run 20261106-202424 (1 variant, 6 credits), prompt `prompts/02-correction-1.txt`. References: variant 1 of the scene run, the extractor render.
```
Keep this exact image: same art style, same flat-shaded low-poly look, same camera angle, same 16:9 composition, same shuttle position in the upper right, same miner crouching in the open side hatch, same small floating drone with its orange camera eye beside the hatch, same dark calm bottom left area, same indigo violet magenta nebula, same starfield, same floating grey-violet asteroids, same large cratered planet on the right. No text, no UI. Image 1 is the image to correct. Image 2 is the run shuttle design, used only for the shuttle details.
Only change these four things:
1. The shuttle matches image 2: an orange dome on the roof centre, dense rust chips on the white and teal plates, all four ducted rotor rings visible.
2. The rock blocks on the asteroid have smooth light lilac tops with no pits and no cracks, and a thin glowing orange-yellow rim on the chamfered top edge.
3. The open floor of the asteroid is hand-painted cartoon flagstones with a magenta-purple base and dark cracks with a pink rim highlight; no small rubble, chips or pebbles on the floor; crystals only grow from the rock blocks.
4. The planet is deep ultramarine and violet with no cyan rim glow.
Change nothing else.
```

## 3. Correction 2 (verbatim), retained
Run 20261106-202627 (1 variant, 6 credits), prompt `prompts/03-correction-2.txt`. References: correction 1, the extractor render.
```
Keep this exact image: same art style, same flat-shaded low-poly look, same camera angle, same 16:9 composition, same shuttle with its orange roof dome, rusty white and teal plates, amber cockpit and landing legs, same miner crouching in the open side hatch, same small floating drone with its orange camera eye beside the hatch, same asteroid with smooth lilac rock blocks with a thin orange-yellow rim, same magenta-purple cartoon flagstone floor, same violet crystals, same dark calm bottom left area, same nebula, starfield, floating asteroids and violet planet. No text, no UI. Image 1 is the image to correct. Image 2 is the run shuttle design: it has four ducted rotors, one at each corner.
Only change these two things:
1. Add the fourth ducted rotor of the shuttle on the near front corner, below and in front of the cockpit side, as in image 2, placed so it does not cover the open hatch, the miner or the drone.
2. The single pale blue-grey flagstone right of the centre of the floor becomes magenta-purple like the other flagstones.
Change nothing else.
```

## Style review (style-guide agent)
- Scene run: both fail: rock without the orange chamfer rim and pitted tops (color-code), floor not cartoon flagstones (A13), loose chips (A14), shuttle missing roof dome, rust and a rotor; variant 2 also blue drone eyes and a busy bottom left. Variant 1 kept.
- Correction 1: every fail fixed except the fourth rotor; one pale blue-grey flagstone (A13).
- Correction 2: pass; four rotors, flagstone fixed, no regression.
- Open deviation, for the owner: A12, low three-quarter camera, miner small at the hatch, as for the main menu background ([[art-prompts-main-menu-background]]).

## Relations
- [[art-prompts]] — parent: method and locked phrases.
- [[intermission]] — the loading screen look.
- [[task-488-01-loading-image]] — the subtask that made it.
- [[art-prompts-main-menu-background]] — style reference and first image.
- [[art-prompts-extractor]] — the shuttle mesh the reference render comes from.
