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

## Relations
- [[art-prompts]] — parent: method and prompt nodes.
- [[art-prompts-level-end-screen]] — the ChatGPT prompt these images answer.
- [[art-prompts-shop-screen-mockup]] — Spare Parts sprites and method this follows.
- [[task-380-level-end-frame-sprites]] — the task that makes the sprites.
