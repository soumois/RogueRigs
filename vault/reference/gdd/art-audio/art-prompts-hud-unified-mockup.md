---
id: art-prompts-hud-unified-mockup
description: "Unified HUD mockups (task-451), radar, alert bar, bottom panel and XP frame sprites (453, 457, 461): Kie.ai prompts, runs, credits, critic reviews, labels, margins."
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

## Alert bar sprites (task-453-01)
- Prompt `prompts/08-alert-bar.txt`, refs: a 3x crop of the mockup alert bar (Image 1, temporary, `Saved/Scratch/hud-alert/mockup-alert-crop.png`) and the mockup (Image 2). GPT Image 2 image-to-image, 16:9, 1K, magenta key, 2 variants, 12 credits. Run `20261105-164216-hud-unified`. One sheet per variant: the gauge frame with a hollow track window (top) and one swarm mark, red "!" in a dark capsule tab (bottom).
- Each sheet split at y 470, keyed with `Tools/art/ui_icon_prepare.js --keep-aspect` (frame `--size 1024 --margin 0.01`, mark `--size 256 --margin 0.04`), `--check` clean.
- Critic (`20261105-164216-hud-unified.review.json`): both pass A1, A2, A6, A7, A14; candidate 1 closer to the mockup (slim rounded capsule, light rust, brass rivets). Reminder on both: the red "!" is a HUD use of red (readability-rules); owner kept it, no colour-code change.
- Owner 2026-11-05 (verbatim): "Candidat 1". Keep = `20261105-164216-hud-unified-1.png`, its log and review; temporary = `-2.png`, moved to `Saved/Scratch/hud-unified/`. Cut-outs: `Art/UI/HUD/T_HUD_AlertFrame.png` (1024 x 131 RGBA, frame 1004 x 111 at 10,10, transparent window) and `Art/UI/HUD/T_HUD_AlertMark.png` (124 x 256 RGBA). C, B, S stay UMG letters, as in the mockup.

## Weapon slot frame sprite (task-458-01)
- No new sprite, no Kie.ai run, no credit (main session 2026-11-05: reuse a kept sprite that fits, as task-452 did). Each slot frame of the mockup (dark inside, gunmetal edges with small rivets, teal chamfered corner plates, rust chips) is the look of the owner-retained `Art/UI/LevelEnd/T_LevelEnd_Frame.png` (942 x 458 RGBA, keyed, task-380-02, [[art-prompts-level-end-screen-mockup]]), imported as `/Game/RogueRigs/UI/LevelEnd/T_LevelEnd_Frame`; its Draw As Box margins are measured there (0.0955 / 0.1878).
- The bolted base around the four slots reuses `T_Shop_ButtonReroll`, the plate of the health panel (task-452), so the health plate and the slot row read as one block.

## Bottom panel and XP frame sprites (task-461-01)
- Q37 (owner "ok" 2026-11-06): two new Kie.ai sheets from the retained mockup, 2 variants each, critic keeps the closest. GPT Image 2 image-to-image, 16:9, 1K, magenta key, 24 credits.
- Prompt `prompts/09-bottom-panel.txt` (run `20261105-202435-hud-unified`): one empty riveted panel (health and four slots go inside in UMG) and one slot well. Refs: 2.4x crop of the mockup bottom-centre piece (Image 1, temporary, `Saved/Scratch/hud-bottom/mockup-panel-crop.png`), the mockup (Image 2).
- Prompt `prompts/10-xp-frame.txt` (run `20261105-202647-hud-unified`): XP bar frame with a hollow track window, and an empty level plate. Refs: 2.8x crop of the mockup bottom-left (temporary, `Saved/Scratch/hud-bottom/mockup-xp-crop.png`), the mockup.
- Sheets split and keyed with `Tools/art/ui_icon_prepare.js --keep-aspect`, `--check` clean (no magenta, no pink spill).
- Critic (`<run-id>.review.json`): all four pass A1, A2, A6, A7, A14, colour code, readability. Picks c1 on both runs (closest to the mockup: darker gunmetal, rust streaks). Notes: rivets along the panel long edges stretch in a 9-slice (drawn near the source aspect to limit it); the XP frame teal is lighter than the panel.
- Labels: keep = `20261105-202435-hud-unified-1.png`, `20261105-202647-hud-unified-1.png`, their logs, reviews and prompts 09, 10; temporary = both `-2.png`, moved to `Saved/Scratch/hud-unified/`, and the cut candidates in `Saved/Scratch/hud-bottom/`.
- Cut-outs in `Art/UI/HUD/` (RGBA), Draw As Box margins measured on the image:

| Texture | Size | Margins L/R, T/B | Inside |
|---|---|---|---|
| T_HUD_BottomPanel | 1024 x 273 | 0.095, 0.28 | dark plate face |
| T_HUD_SlotWell | 512 x 198 | 0.14, 0.28 | near-black |
| T_HUD_XPFrame | 1600 x 153 | 0.06, 0.3 | transparent track window y 43..109 |
| T_HUD_LevelCap | 512 x 179 | 0.2, 0.3 | near-black |

## Thin bottom frames (task-467-01, replace the 461 sprites)
- Q2 of [[task-467-bottom-hud-match-target]] (owner 2026-11-06, default): owner rejected the 461 look (bulky panel, big bolt heads, rounded wells, arrow XP ends). Two new sheets from crops of the retained mockup, 2 variants each, GPT Image 2 image-to-image, 16:9, 1K, magenta key, 24 credits.
- Prompt `prompts/11-bottom-block-frames.txt` (run `20261106-015539-hud-unified`): hollow health strip frame, slim block frame with a dark inside, one flat cell. Refs: 2.5x crop of the mockup bottom block (temporary, `Saved/Scratch/hud-467/target-block-target.png`), the mockup.
- Prompt `prompts/12-xp-rail.txt` (run `20261106-015950-hud-unified`): straight rail with square ends and a hollow window, empty Lv plate. Refs: 3x crop of the mockup bottom-left (temporary, `Saved/Scratch/hud-467/target-xpleft-target.png`), the mockup.
- Critic (style-guide, task-467-01): run A both pass; picks variant 1 (blue-grey gunmetal, small brass rivets). Run B variant 1 fails A1 (glossy tubes, chrome dome bolts); variant 2 passes and is picked. Notes: block frame thinner than the mockup; Lv plate has 4 rivets, the mockup 8.
- Labels: keep = `20261106-015539-hud-unified-1.png`, `20261106-015950-hud-unified-2.png`, both logs, prompts 11, 12; temporary = `20261106-015539-hud-unified-2.png`, `20261106-015950-hud-unified-1.png` (moved to `Saved/Scratch/hud-unified/`), the split and cut candidates in `Saved/Scratch/hud-467/`.
- Cut-outs in `Art/UI/HUD/` (`ui_icon_prepare.js --keep-aspect`, margin 0.002 to 0.004, `--check` clean), cut at the pixel size they are drawn at 1080p: in game the Box border is drawn at the texture's own pixel size, not at the brush ImageSize (task-467-02 capture; the 461 sprites, 512 to 1600 px wide, drew huge borders and bolts for this reason). Draw As Box margins measured on the image:

| Texture | Size | Margins L/R, T/B | Inside |
|---|---|---|---|
| T_HUD_HealthStrip | 470 x 45 | 0.028, 0.22 | transparent window y 9..35 |
| T_HUD_BlockFrame | 750 x 141 | 0.03, 0.16 | near-black |
| T_HUD_SlotCell | 160 x 82 | 0.075, 0.15 | near-black |
| T_HUD_XPRail | 840 x 58 | 0.031, 0.31 | transparent window y 18..40 |
| T_HUD_LevelPlate | 216 x 55 | 0.065, 0.22 | near-black |

## Relations
- [[art-prompts]] — parent: method and prompt nodes.
- [[task-467-bottom-hud-match-target]] — the task that made the thin bottom frames.
- [[art-prompts-level-end-screen-mockup]] — T_LevelEnd_Frame, reused for the weapon slot frames (task-458).
- [[wbp-alert-objectives]] — shows the alert bar sprites (task-453).
- [[task-451-hud-mockups]] — the task that made these mockups.
- [[art-prompts-shop-screen-mockup]] — the style reference image.
- [[kie-image-workflow]] — tool, credits and commit rule.
- [[wbp-minimap-locator]] — shows the radar frame sprite (task-457).
- [[wbp-weapons]] — shows the weapon slot frame and base sprites (task-458).
- [[wbp-xp-status]] — shows the XP frame and level cap sprites (task-461).
- [[wbp-health-status]] — shows the bottom panel and the health track well (task-461).
