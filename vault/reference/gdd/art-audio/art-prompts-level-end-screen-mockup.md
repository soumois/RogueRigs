---
id: art-prompts-level-end-screen-mockup
description: "ChatGPT level-end screen mockup and asset sheet brought by the owner 2026-10-30: files, contents, elements taken, 3x3 method, budget. Open before level-end art."
parent: art-prompts
kind: note
---
# Level-end screen — ChatGPT mockup and asset sheet

Answer of ChatGPT to the prompt of [[art-prompts-level-end-screen]], brought by the owner 2026-10-30 with: "Je te laisse utiliser l'api pour reprendre les éléments que tu as besoin. N'oublie pas de faire les cadres avec le système 3x3". Used by [[task-380-level-end-frame-sprites]].

## Files (owner images, saved as received, no conversion)
- `Art/Source/level-end-screen/reference/chatgpt-level-end-mockup-2026-10-30.webp` (1672 x 941): full screen. Title "LEVEL 1 CLEARED" with riveted side bars; subtitle "NEXT UP: LEVEL 2 OF 4" between thin lines; a riveted teal and gunmetal frame holding a "Total Ore: 61" band (ore icon) on top, then two columns "Robots destroyed" (Chaser 8, Shooter 5, Heavy 2, Kamikaze 3) and "Damage dealt" (Burst rifle 125), each under a thin line; a cream rusty "NEXT LEVEL" button with teal ends; the arena dimmed almost black behind, violet ore crystals in the corners.
- `Art/Source/level-end-screen/reference/chatgpt-level-end-asset-sheet-2026-10-30.webp` (1500 x 844): about 20 groups: title bar and its modular parts and states, main panel, modular panel components (4 corners, 4 edges), panel variants (default, hovered, disabled), single-row stats sub-panel, two-column stats panel, divider components, corner close-ups, NEXT LEVEL / REROLL / BUY states, duskite ore icons and effects, misc elements, text styles.

## Notes
- The sheet is too small to cut final sprites from (about 1500 x 844 for 20 groups): the sprites are regenerated, as in [[art-prompts-shop-screen-mockup]].
- The mockup frame and its dividers carry bolts and tabs in the middle of the edges: a 9-slice would stretch them.
- The sheet labels the ore "duskite": not in the GDD, not taken.

## Elements (decided in [[task-380-level-end-frame-sprites]], owner "Ok, par défaut" 2026-10-30)
- New sprites (Q2): the main frame (teal corner brackets, riveted gunmetal edges), a horizontal divider and a vertical divider with bolted ends. The single-row sub-panel and the two-column panel of the sheet are not separate sprites (the dividers split the frame).
- Reused: `T_Shop_TitleBar` (title side bars), `T_Shop_ButtonContinue` for NEXT LEVEL (hover, pressed, disabled by UMG tint), `T_Icon_Ore`.
- Not taken: REROLL and BUY, ore icons and effects, panel variants hovered and disabled, misc elements, the thin lines beside the subtitle and under the column headers (plain UMG lines).

## 3x3 method (Q3, Q4)
- One PNG per frame. Only the corners carry details (teal brackets, bolts); the four edges are uniform strips with no bolt or tab; the centre is a flat dark plate. Not the 9 separate pieces of the sheet modular components.
- Margins measured in px and written with their UMG fraction (Draw As Box, as WBP_ShopScreen). Dividers are 3-slice (fixed bolted ends, uniform middle).
- This screen only: `T_Shop_Panel` is not remade, no rule node is edited.
- Planned sprites in `Art/UI/LevelEnd/`: `T_LevelEnd_Frame.png`, `T_LevelEnd_DividerH.png`, `T_LevelEnd_DividerV.png`.

## Budget (Q5)
Kie.ai GPT Image 2 image-to-image, refs the sheet (Image 1) and the mockup (Image 2), 1K, magenta key, 2 variants per sprite, 6 credits each: 36 credits planned (3 sprites), 72 at most with one correction run per art subtask. The ChatGPT images count as the owner's own design reproduced closely. Every candidate goes to the owner. Gunmetal and teal without white plates accepted as for Spare Parts. Sprites only, no widget change (Q1). Subtask 01 (this archive) spends no credit.

## Sprites
### T_LevelEnd_Frame (task-380-02)
- File: `Art/UI/LevelEnd/T_LevelEnd_Frame.png`, 942 x 458 RGBA: candidate 2 keyed (1024 x 540) with the 41 px transparent pad cropped to the alpha bounding box.
- Prompt: `Art/Source/level-end-screen/prompts/01-main-frame.txt`. Run `20261029-173616-level-end-screen` (log `Art/Source/level-end-screen/20261029-173616-level-end-screen.json`), 2 variants, 12 credits, no correction run.
- Review: `Art/Source/level-end-screen/20261029-173616.review.json`. Candidate 2 chosen by the owner as is (raw `20261029-173616-level-end-screen-2.png`, keyed `-2-keyed.png`).
- Margins measured by pixels on the cropped file (column and row profile against the middle strip, where the edge becomes uniform): left 90 (87 to 90 by corner), right 90, top 86 (84 to 86), bottom 86.
- Draw As Box margins (margin / texture size): L and R 90 / 942 = 0.0955; T and B 86 / 458 = 0.1878. The earlier eye guess was about 92 px L/R and 81 px T/B: L/R confirmed, T/B 5 px larger.
- Gaps accepted by the owner: rivets along the four edges stretch into ovals when the frame is scaled (the brief asked for none); A6 and A7 reminders accepted.

### T_LevelEnd_DividerH and T_LevelEnd_DividerV (task-380-03)
- Files: `Art/UI/LevelEnd/T_LevelEnd_DividerH.png` 984 x 62 RGBA (H1 keyed, 1024 x 102 cropped to the alpha bounding box) and `Art/UI/LevelEnd/T_LevelEnd_DividerV.png` 58 x 984 RGBA (V1 keyed, 98 x 1024 cropped). `ui_icon_prepare.js --check`: corners transparent, no magenta or pink spill.
- Prompts: `Art/Source/level-end-screen/prompts/02-divider-h.txt` (run `20261029-174247-level-end-screen`, log `Art/Source/level-end-screen/20261029-174247-level-end-screen.json`) and `Art/Source/level-end-screen/prompts/03-divider-v.txt` (run `20261029-174426-level-end-screen`, log `Art/Source/level-end-screen/20261029-174426-level-end-screen.json`). 2 variants each, 12 credits per run, 24 credits total, no correction run.
- Retained: H1 (raw `20261029-174247-level-end-screen-1.png`, keyed `-1-keyed.png`) and V1 (raw `20261029-174426-level-end-screen-1.png`, keyed `-1-keyed.png`), both chosen by the owner as is. V2 rejected: shading break.
- Reviews: `Art/Source/level-end-screen/20261029-174247.review.json`, `Art/Source/level-end-screen/20261029-174426.review.json`.
- End-cap margins measured by pixel profile on the cropped files (column or row mean against the middle, where the cap becomes uniform; tolerance 10 of 255, 6 gives 70/67 and 76/57): H left 68, right 65 of 984, fractions 0.069 and 0.066, short side 62; V top 56, bottom 57 of 984, fractions 0.057 and 0.058, short side 58. The uniform middle has alpha about 165 to 171 of 255 (semi-transparent body, caps about 250).
- Draw As Box (margin / texture size): H left 0.069, right 0.066 (top and bottom 0); V top 0.057, bottom 0.058 (left and right 0).
- Gaps accepted by the owner: all four candidates failed A6 (cleaner than the frame, no rust, no rivets).

## Relations
- [[art-prompts]] — parent: method and prompt nodes.
- [[art-prompts-level-end-screen]] — the ChatGPT prompt these images answer.
- [[art-prompts-shop-screen-mockup]] — Spare Parts sprites and method this follows.
- [[task-380-level-end-frame-sprites]] — the task that makes the sprites.
- [[art-prompts-hud-unified-mockup]] — reuses T_LevelEnd_Frame for the HUD weapon slot frames (task-458).
