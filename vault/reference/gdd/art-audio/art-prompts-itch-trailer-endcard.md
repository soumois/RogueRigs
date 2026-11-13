---
id: art-prompts-itch-trailer-endcard
description: "itch.io trailer end-card key art: Kie.ai scene prompt, one correction, references, runs, credits, style review. Open to redo or reuse for store art."
parent: art-prompts
kind: note
---
# itch.io trailer end card — prompts and review

Marketing illustration, not an in-game asset. Asked by the owner on 2026-11-14 to replace the loading screen illustration ([[art-prompts-loading-screen]]) on the itch.io trailer end card, because that image has an artefact. Model `gpt-image-2` through Kie.ai ([[kie-image-workflow]]), 16:9, 1K (1672 x 941). The title "ROGUE RIGS" and "PLAY NOW ON ITCH.IO" are overlaid on the empty left side when the video is edited; the image itself has no text. Retained image: `Art/Source/itch-trailer-endcard/20261114-003551-itch-trailer-endcard-1.png`. Status: keep. Total 18 credits over 2 runs.

## 1. Scene
Run 20261114-003304 (2 variants, 12 credits). The prompt is in `Art/Source/itch-trailer-endcard/prompts/01-scene.txt` (verbatim, also in the run log `.json`). References, in order: `vault/assets/main-menu-background.png`, `keyart.jpg`, `rigs-lineup.jpg`, `enemies-lineup.jpg`. It shows the miner, drone and turret making a last stand against a robot wave on the arena asteroid, with the left 40 percent left empty for the title. Variant 2 kept as the base. Variant 1 is temporary and sits in `Saved/Scratch/itch-trailer/` (the cliff and asteroids fill the title zone).

## 2. Correction 1, retained
Run 20261114-003551 (1 variant, 6 credits). The prompt is in `prompts/02-correction-1.txt`. References: scene variant 2, `rigs-lineup.jpg`. It fixes four things: the turret now has a three-leg tripod and green laser bolts, the explosions are cartoon, the floor is clean, and there are no asteroids in the left 40 percent.

## Style review (style-guide agent)
- Scene: variant 2 was preferred. Fails: white-gold turret fire (color-code), turret spider legs (A4), rubble (A14), realistic explosions (A10), asteroids in the title zone.
- Correction 1: those five are fixed, with no regression. It is usable as is.
- Open deviations, for the owner, accepted for a marketing shot: the miner is seen from behind so the visor is hidden (A6); the shooter has no arm cannon or antenna (A3); the camera is low three-quarter (A12); the floor is plain facets rather than cartoon flagstones (A13).

## Relations
- [[art-prompts]] — parent: method and locked phrases.
- [[art-prompts-loading-screen]] — the image this one replaces on the trailer end card.
- [[kie-image-workflow]] — the generation tool.
