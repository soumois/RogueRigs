---
id: art-prompts-hud-unified-mockup
description: "Unified in-game HUD mockups (task-451): Kie.ai prompts, runs, credits, critic reviews, candidate files, owner choice and what tasks 452-458 take."
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

Labels: all nine are candidates, untracked until the owner chooses. The retained image becomes keep (committed with its log); the others become temporary and move to `Saved/Scratch/hud-unified/`.

## Critic review (style-guide)
- No A rule or color-code fail on any image; green health fill noted only (player expectation, colour rules not rigid).
- Round 1: all six fail the brief: C, B, S hang under the gauge; XP ticks 5 or 6 instead of 4; A-1 invents an "E" marker; C-1 and C-2 look like direction A; C-1 keeps the mouse cursor.
- Correction: A3 passes everything except tick spacing (4 ticks, about 18/37/57/75 %). B3 has markers on the track but 5 ticks and tiny yellow specks on a rock. C3 is slim with 4 ticks, but its markers still hang off the gauge and a gap separates health from the slots.
- Gaps left: the image model does not place ticks exactly; UMG will place them at 20/40/60/80 %.

## Owner choice
- open (waiting for the owner, 2026-11-05).

## What tasks 452-458 take
- open: written from the retained image.

## Relations
- [[art-prompts]] — parent: method and prompt nodes.
- [[task-451-hud-mockups]] — the task that made these mockups.
- [[art-prompts-shop-screen-mockup]] — the style reference image.
- [[kie-image-workflow]] — tool, credits and commit rule.
