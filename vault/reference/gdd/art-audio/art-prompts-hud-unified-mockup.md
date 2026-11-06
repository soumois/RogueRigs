---
id: art-prompts-hud-unified-mockup
description: "Unified in-game HUD mockups (task-451) and the radar frame sprite (task-457): Kie.ai prompts, runs, credits, critic reviews, candidates, owner choice, what 452-458 take."
parent: art-prompts
kind: note
---
# Unified in-game HUD mockups (Kie.ai, task-451-01)

Request and decided content: [[task-451-hud-mockups]] (`request`, Q24, Q33). Method: [[kie-image-workflow]], GPT Image 2 image-to-image, 16:9, 1K, 6 credits per image.

## References
- Image 1 (layout and scene): `Art/Source/hud-unified/reference/owner-hud-current-2026-11-05.webp`.
- Image 2 (material style only): `Art/Source/shop-screen/reference/chatgpt-shop-mockup-2026-10-28.webp` ([[art-prompts-shop-screen-mockup]]), for the Kessler-Voss shop look.
- `Art/Source/hud-unified/reference/external-example-radar-panel-2026-11-05.png` was not attached: the owner's example is an idea only, and attaching it risks a copy; its idea is written into the prompt (radar frame running on into an attached ore plate).

## Prompts (verbatim, `Art/Source/hud-unified/prompts/`)
- `01-direction-a-riveted-plates.txt`, `02-direction-b-pipework.txt`, `03-direction-c-slim-visor.txt`: shared HUD brief plus one direction paragraph; only texts "0", "!", "C", "B", "S", "100 / 100", "Lv 1".
- `04-direction-a-correction.txt`, `05-direction-b-correction.txt`: "Keep this exact image", markers on the gauge track, 4 XP ticks.
- `06-direction-c-slim-visor-v2.txt`: direction C rerun with thin rims and smoked glass stated harder, plus the same two fixes.

## Runs and candidates (`Art/Source/hud-unified/`, log `<run-id>.json` beside each)
| Direction | Round 1 (2 variants) | Correction (1 variant) |
|---|---|---|
| A Riveted plates | `20261105-151729-hud-unified-1.png`, `-2.png` | `20261105-152627-hud-unified-1.png` (from A-2) |
| B Pipework | `20261105-152208-hud-unified-1.png`, `-2.png` | `20261105-152746-hud-unified-1.png` (from B-2) |
| C Slim visor | `20261105-152333-hud-unified-1.png`, `-2.png` | `20261105-152849-hud-unified-1.png` (new run) |

Credits: 36 (round 1) + 18 (correction) = 54, over the "about 36" of Q24 by the one correction round of the executor rule.

Labels (2026-11-05, after the owner's choice): keep = `20261105-152627-hud-unified-1.png` and its log `20261105-152627-hud-unified.json`, committed in `Art/Source/hud-unified/`; temporary = the eight others and their logs, moved to `Saved/Scratch/hud-unified/` (git-ignored), including its source image A-2 (`20261105-151729-hud-unified-2.png`) that the kept log names as Image 1.

## Critic review (style-guide)
- No A rule or color-code fail on any image; green health fill noted only (player expectation, colour rules not rigid).
- Round 1: all six fail the brief: C, B, S hang under the gauge; XP ticks 5 or 6 instead of 4; A-1 invents an "E" marker; C-1 and C-2 look like direction A; C-1 keeps the mouse cursor.
- Correction: A3 passes everything except tick spacing (4 ticks, about 18/37/57/75 %). B3 has markers on the track but 5 ticks and tiny yellow specks on a rock. C3 is slim with 4 ticks, but its markers still hang off the gauge and a gap separates health from the slots.
- Gaps left: the image model does not place ticks exactly; UMG will place them at 20/40/60/80 %.

## Owner choice
- Owner 2026-11-05 (verbatim, shown the three corrected images): "A, plaques rivetées". Retained: `Art/Source/hud-unified/20261105-152627-hud-unified-1.png` (direction A, corrected). No correction requested.

## What tasks 452-458 take
The retained image is the layout and look reference; every piece uses one frame family: riveted gunmetal plates, worn teal trims, rust chips, chamfered corners, dark insides. Exact sizes and ticks are set in UMG ([[umg-layout]]), not measured on the image.
- 452 health: green bar with "100 / 100" in a riveted plate the width of the weapon row, sitting directly on the weapon block, one bolted base.
- 453 alert bar: riveted gauge top centre, orange fill; the two red "!" and C (cyan), B (yellow), S (white) set on the track itself, nothing hanging under it.
- 454 swarm rule: nothing (gameplay rule, no visual).
- 455 XP ticks: bottom-edge riveted bar with a "Lv 1" plate at its left; 4 light ticks at 20/40/60/80 % (the image spaces them unevenly, about 18/37/57/75 %: do not copy).
- 456 XP ghost: same XP bar frame; the image shows no ghost effect.
- 457 radar and ore: riveted ring around the round minimap, the frame running right into an attached plate with the crystal icon and the count.
- 458 weapon slots: 4 slots in one bolted frame, rifle with its cream reload bar, teal padlocks on locked slots.

## Radar frame sprite (task-457-01)
- Prompt `prompts/07-radar-frame.txt`, refs: a 3x crop of the mockup top-left piece (Image 1, temporary, `Saved/Scratch/hud-radar/mockup-radar-crop.png`) and the mockup (Image 2); the external example not attached. GPT Image 2 image-to-image, 16:9, 1K, magenta key, 2 variants, 12 credits. Run `20261105-155448-hud-unified`.
- Candidates `-1.png` and `-2.png`. Keyed with `Tools/art/ui_icon_prepare.js --size 1024 --keep-aspect --margin 0.02`, `--check` clean, to `Saved/Scratch/hud-radar/T_HUD_RadarFrame-c1.png` (1024 x 580, hole 386 x 381 centred at 304,288) and `-c2.png` (1024 x 560, hole 385 x 374 at 292,278).
- Critic: both pass A1-A18 and the colour code; neither copies the external example. Both are more teal than the mockup's dark gunmetal plates; c1 is the closer one (c2 rust more saturated).
- Owner 2026-11-05 (verbatim): "Candidat 1". Keep = `Art/Source/hud-unified/20261105-155448-hud-unified-1.png` and its log `20261105-155448-hud-unified.json`; temporary = `-2.png`, moved to `Saved/Scratch/hud-unified/`. Cut-out copied to `Art/UI/HUD/T_HUD_RadarFrame.png` (1024 x 580 RGBA): hole centre 304,288, diameter about 383; plate dark inside x 578..928, y 122..279.

## Relations
- [[art-prompts]] — parent: method and prompt nodes.
- [[task-451-hud-mockups]] — the task that made these mockups.
- [[art-prompts-shop-screen-mockup]] — the style reference image.
- [[kie-image-workflow]] — tool, credits and commit rule.
- [[wbp-minimap-locator]] — shows the radar frame sprite (task-457).
