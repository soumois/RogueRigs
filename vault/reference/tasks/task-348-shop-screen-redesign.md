---
id: task-348-shop-screen-redesign
description: "Global task: Rebuild the shop screen to the ChatGPT mockup look, making only the sprites the game lacks; stat icons and card frames stay."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Shop screen redesign after the ChatGPT mockup

- request: "Voilà la proposition de chatgpt. Ne reprend pas les icones, ne reprend que ce qu'il te manquait pour faire l'écran." (owner, 2026-10-28, with the mockup and the asset sheet saved in [[art-prompts-shop-screen-mockup]]; relayed by the main session: the stat icons already in the game stay; take only what is missing to build the new screen.) Owner answer 2026-10-28 (relayed by the main session): "OK, tout par défaut" = every default of Q1 to Q11 accepted.
- goal: WBP_ShopScreen and WBP_ShopCard take the look of the mockup; only the missing sprites are made; stat icons, tier card frames and T_Icon_Ore are kept.
- acceptance: -game at 1920 x 1080: title with side bars and subtitle; ore counter in its pipe panel top left; riveted shop panel holding the 5 cards; a tier tab on each card; styled Buy button and "need N more" when short; sold card with burnt edges and wrench; Reroll and Continue styled under the panel; game rules unchanged; the 342 to 344 purchase effects still play; capture compared with `Art/Source/shop-screen/reference/shop-screen-2026-10-28.png` and the mockup, sent to the owner; docs updated; lint clean; committed.
- gdd_check: read 2026-10-28: [[run-shop]], [[rarities]], [[color-code]], [[readability-rules]], [[art-prompts]], [[art-prompts-shop-screen]], [[agent-art]], [[kie-image-workflow]], [[art-3d-lessons]], tech [[wbp-shop-screen]], [[wbp-shop-card]], [[sys-text-content]]; tasks 342 to 344. Existing: the mockup cards are the current baked stat frame tinted per tier, T_Icon_Ore, the stat icons, MI_CardBurn. Missing: title bars, counter panel, shop panel, button plates, reroll arrow, tier tab, sold look. Conflicts and open values: the sheet (1125 x 750) is too small to cut sprites from; [[kie-image-workflow]] treats an owner-sent third-party image as reference only; the mockup writes COMMON in blue while grey is common and blue is rare ([[rarities]]); tier names are not in the GDD; the mockup drops the target line and the red short price. Questions (defaults, values provisional): see `## Questions`. Owner answer 2026-10-28: "OK, tout par défaut": every default of `## Questions` is DECIDED (values provisional, owner 2026-10-28); [[run-shop]] updated by the Planner the same day. Order: starts only after [[task-344-shop-card-burn]] is done (Q11).
- confirmed_by_owner: yes (2026-10-28, "OK, tout par défaut" to Q1-Q11 and the plan, relayed by the main session)
- milestone: M2
- next_step: task-348-02-shop-button-sprites
- started: 2026-10-28T15:40:51Z
- waiting_for: owner choice of sprite candidates of subtask 01
- result: open

## Questions
- Q1 Sprites regenerated clean by the art agent (GPT Image 2 image-to-image on Kie.ai, the sheet as reference, magenta key), not cut from the sheet; the ChatGPT images count as the owner's own design, reproduced closely; at most 2 variants and 1 correction run per art subtask (about 72 credits in all).
- Q2 Taken: title side bars, counter panel with pipes, shop panel, Buy, Reroll and Continue plates, reroll arrow icon, tier tab, sold burnt panel, wrench icon. Not taken: stat icons, tier frames, ore crystal, crystal animation frames, group 11 effects (342 to 344 keep their assets), empty slot, counter variants, spare corners.
- Q3 One plate per button; hover, pressed and disabled made by UMG tint.
- Q4 Texts after the mockup, upper case: subtitle "RIG SERVICE TERMINAL", "BUY", "NEED {n} MORE" (price minus balance) instead of "Short on ore", "SOLD" / "NO REFUNDS.", "REROLL", "CONTINUE"; tab names COMMON, UNCOMMON, RARE, EPIC, LEGENDARY.
- Q5 Tab and its label in the tier colour (common grey, not blue).
- Q6 The target line ("Burst rifle", "All weapons") stays.
- Q7 Price always ore violet (0.85, 0.45, 1.0); shortage shown by the grey button and the dim card, no red.
- Q8 Font stays Roboto Bold; the title stays a text.
- Q9 Layout: counter top left, Reroll centred under the panel, Continue under it; the 342 ore flight then starts top left.
- Q10 Sold look: burnt panel in the grey stat frame with wrench, "SOLD", "NO REFUNDS."; it ends the 344 burn.
- Q11 Runs after task-344 is done; widget names and animations of 342 to 344 are kept.

## Subtasks
1. task-348-01-shop-chrome-sprites (vault/reference/tasks/task-348-shop-screen-redesign/task-348-01-shop-chrome-sprites.md) — art: title side bars, ore counter panel, shop panel sprites.
2. task-348-02-shop-button-sprites (vault/reference/tasks/task-348-shop-screen-redesign/task-348-02-shop-button-sprites.md) — art: Buy, Reroll, Continue plates and the reroll arrow.
3. task-348-03-shop-card-sprites (vault/reference/tasks/task-348-shop-screen-redesign/task-348-03-shop-card-sprites.md) — art: tier tab, sold burnt panel, wrench icon.
4. task-348-04-shop-screen-texts (vault/reference/tasks/task-348-shop-screen-redesign/task-348-04-shop-screen-texts.md) — text: shop labels, subtitle and tier names in screen_texts.json.
5. task-348-05-shop-sprites-import (vault/reference/tasks/task-348-shop-screen-redesign/task-348-05-shop-sprites-import.md) — bp: import the sprites as UI textures.
6. task-348-06-shop-card-restyle (vault/reference/tasks/task-348-shop-screen-redesign/task-348-06-shop-card-restyle.md) — bp: WBP_ShopCard to the mockup.
7. task-348-07-shop-screen-restyle (vault/reference/tasks/task-348-shop-screen-redesign/task-348-07-shop-screen-restyle.md) — bp: WBP_ShopScreen to the mockup.
8. task-348-08-shop-screen-capture (vault/reference/tasks/task-348-shop-screen-redesign/task-348-08-shop-screen-capture.md) — cpp: -game capture compared with the old screen and the mockup.
9. task-348-09-shop-screen-doc (vault/reference/tasks/task-348-shop-screen-redesign/task-348-09-shop-screen-doc.md) — doc: update wbp-shop-card, wbp-shop-screen.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-348-01-shop-chrome-sprites]] — step 1, art.
- [[task-348-02-shop-button-sprites]] — step 2, art.
- [[task-348-03-shop-card-sprites]] — step 3, art.
- [[task-348-04-shop-screen-texts]] — step 4, text.
- [[task-348-05-shop-sprites-import]] — step 5, bp.
- [[task-348-06-shop-card-restyle]] — step 6, bp.
- [[task-348-07-shop-screen-restyle]] — step 7, bp.
- [[task-348-08-shop-screen-capture]] — step 8, cpp.
- [[task-348-09-shop-screen-doc]] — step 9, doc.
- [[task-344-shop-card-burn]] — runs after it (Q11).
- [[art-prompts-shop-screen-mockup]] — the owner's mockup and asset sheet.
