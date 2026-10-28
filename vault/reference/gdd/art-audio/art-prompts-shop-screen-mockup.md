---
id: art-prompts-shop-screen-mockup
description: "ChatGPT shop screen mockup and asset sheet brought by the owner 2026-10-28: files, contents, which elements are taken. Open before shop screen art."
parent: art-prompts
kind: note
---
# Shop screen — ChatGPT mockup and asset sheet

Answer of ChatGPT to the prompt of [[art-prompts-shop-screen]], brought by the owner 2026-10-28 with: "Voilà la proposition de chatgpt. Ne reprend pas les icones, ne reprend que ce qu'il te manquait pour faire l'écran." Used by [[task-348-shop-screen-redesign]].

## Files (owner images, saved as received)
- `Art/Source/shop-screen/reference/chatgpt-shop-mockup-2026-10-28.webp` (1672 x 941): full screen. Title "SPARE PARTS" with side bars and the subtitle "RIG SERVICE TERMINAL"; ore counter in a pipe-framed panel top left (balance 7); a large riveted teal and rust shop panel holding 5 cards (4 for sale, tier tabs COMMON, EPIC, UNCOMMON, COMMON; price row in violet; greyed "NEED xx MORE" buttons; slot 5 sold: burnt card edges, wrench icon, "SOLD / NO REFUNDS."); under the panel a "REROLL | ore 20" button, then a cream rusty "CONTINUE" button.
- `Art/Source/shop-screen/reference/chatgpt-shop-asset-sheet-2026-10-28.webp` (1125 x 750): 11 groups: 1 title, 2 ore counter and crystal animation frames, 3 shop panel, 4 card frames per tier, 5 card content and stat icons, 6 card states (normal, unaffordable, sold, empty slot), 7 Buy button states, 8 Reroll states, 9 Continue states, 10 counters, bars and corners, 11 effect sprites (crystal travel, button impact per tier, card burn mask, sparks).

## Notes
- The sheet is too small to cut final sprites from (a card frame is about 110 x 150 px, a button about 120 x 30 px).
- The stat icons are not taken (owner). Elements taken: decided in [[task-348-shop-screen-redesign]] `gdd_check`.

## Sprites (task-348-01)
Kie.ai GPT Image 2 image-to-image, refs: the sheet (Image 1) and the mockup (Image 2), 16:9, 1K, magenta key, 2 variants per sprite, 6 credits each: 36 credits in all, no correction run. Prompts verbatim in `Art/Source/shop-screen/prompts/`: `01-title-bar.txt`, `02-counter-panel.txt`, `03-shop-panel.txt`. Runs `20261028-074147-shop-screen` (bar), `20261028-074308-shop-screen` (counter), `20261028-074429-shop-screen` (panel); keyed with `Tools/art/ui_icon_prepare.js --keep-aspect`, `--check` clean (no magenta, no pink spill).
- Retained by the owner (2026-10-28): title bar candidate 2, counter panel candidate 1, shop panel candidate 2, copied to `Art/UI/Shop/`: `T_Shop_TitleBar.png` (512 x 69, left bar; UMG mirrors it), `T_Shop_CounterPanel.png` (512 x 228, empty inside), `T_Shop_Panel.png` (1024 x 409, empty inside).
- 9-slice of `T_Shop_Panel.png` (estimates, untested in UMG): left and right about 150 px, top and bottom about 75 px.
- A6 note: the gunmetal and teal look with no white plates is accepted on purpose by the owner; no white-plate correction.
- Gaps: the pipes of the counter panel are cut at the image edge (left pair, top right); the shop panel has bolted side tabs that stick out in the middle of its long sides; the panel ratio is about 2.5:1, not 8:3.

## Sprites (task-348-02)
Same method as task-348-01: Kie.ai GPT Image 2 image-to-image, refs the sheet and the mockup, 1K, magenta key, 2 variants per sprite, 6 credits each: 48 credits in all, no correction run. Prompts verbatim in `Art/Source/shop-screen/prompts/`: `04-button-buy.txt`, `05-button-reroll.txt`, `06-button-continue.txt` (16:9) and `07-icon-reroll.txt` (1:1). Runs `20261028-080238-shop-screen` (Buy), `20261028-080359-shop-screen` (Reroll), `20261028-080539-shop-screen` (Continue), `20261028-080700-shop-screen` (icon). Keyed with `Tools/art/ui_icon_prepare.js --keep-aspect` (icon: `--margin 0.06`), `--check` clean.
- Retained by the owner (2026-10-28), candidate 2 of each, copied to `Art/UI/Shop/`: `T_Shop_ButtonBuy.png` (512 x 137, teal), `T_Shop_ButtonReroll.png` (640 x 180, dark teal, bolted ends), `T_Shop_ButtonContinue.png` (640 x 163, cream rusty metal, teal ends), `T_Shop_IconReroll.png` (256 x 256, light grey).
- 9-slice, horizontal only (estimates, untested in UMG): left and right about 78 px for Buy, about 105 px for Reroll, about 95 to 105 px for Continue; top and bottom 0. Each PNG has a transparent margin of 20 px (Buy) or 26 px (Reroll, Continue).
- Gaps: the plates are taller than the targets of the subtask (Buy 137 vs about 96, Reroll 180 vs about 112, Continue 163 vs about 128): set the height in UMG. Buy c1 (103 high, thin frame) was not retained. The A1 fail of Buy c1 is not kept (unretained).

## Relations
- [[art-prompts]] — parent: method and prompt nodes.
- [[art-prompts-shop-screen]] — the ChatGPT prompt these images answer.
- [[run-shop]] — the shop rules the screen shows.
- [[task-348-shop-screen-redesign]] — the task that rebuilds the screen from them.
