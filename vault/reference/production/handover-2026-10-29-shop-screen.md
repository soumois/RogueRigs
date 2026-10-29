---
id: handover-2026-10-29-shop-screen
description: "State after the ore carry and Spare Parts shop screen session: tasks 340 to 357, decisions, open defects, unseen states, repo state. Open first when resuming."
parent: production-index
kind: note
---
# Handover 2026-10-29: ore carry and Spare Parts shop screen

## Owner intent (his words, summarised)
- Purple crystals (ore) are no longer reset between levels; they live for the current run only, so ore can be accumulated over two levels for a big purchase.
- Spare Parts screen (between two levels): a ChatGPT design prompt, crystals flying from the balance to the Buy button, a juicy Buy click, and the level-up card burn without the travel to the weapon or XP bar.
- Then: rebuild the screen from the ChatGPT mockup (sprites redrawn, icons kept).
- Last: the owner prefers to retouch by hand and wants Canvas Panels with anchors so the screen adapts to small screen-size changes. The resource indicator must centre on the font size, not sit in a Row.

## Tasks (all committed; statuses checked in the task nodes)
- 340 ore-run-carry: done. Bank cleared at run end by `ResetRun` (`ed63c5c3`), `WBP_OreCounter` shows carried plus secured (`67076453`), doc (`0ddf4a8e`).
- 341 shop-design-prompt: done (`ad967eaa`, `b181fc7c`). Prompt: `Art/Source/shop-screen/prompts/chatgpt-shop-design.md`.
- 342 shop-ore-flight: done. `OnSlotBought` event (`da3b826e`), card hold (`bad1c58a`), flight (`974d68c6`), doc (`2da7b290`).
- 343 shop-buy-juice: done (`b511200c`, `8d093514`).
- 344 shop-card-burn: done (`484985ee`, `6d56b0f1`).
- 348 shop-screen-redesign: done. Sprites `9df523af`, `cc839ca7`, `30b7b8d7`; texts `cdaa459c`; import `82132b17`; card `fbbf8ca2`; screen `d904cb8b`; doc `26fcac60`.
- 351 title clip (`fcc11788`, `aa6a8143`), 352 Continue fit (`eb75ca95`, `c39fa2c3`), 353 NEED label (`dde9295e`, `b6341f05`, doc `3e9f336f`, `a6a52459`): done.
- 354 counter overlap: cancelled, absorbed by 356 and 357.
- 355 card frame details: done (`9c59e766`, `f90d2e6d`, close `bf2aa4d4`).
- 356 shop-screen-anchors: done. Owner's manual edits of `WBP_ShopScreen` committed as found (`0610de2d`), Root Canvas rebuild (`18d21409`), card fills its slot (`97a53eae`), doc (`bb3e8fab`), `--force-res` capture flag (`c1a0124b`).
- 357 shop-resource-indicator: done (`e6cbb941`, `08749242`).

## Owner decisions
- "OK, tout par défaut" for Q1 to Q11 (340 to 344 and 348), Q1 to Q9 (351 to 355) and Q1 to Q7 (356, 357). Values are provisional. Q8 (yellow burn flames) and Q9 (square Buy flash) of the 351-355 plan got no task.
- Sprites kept: title bar c2, counter panel c1, shop panel c2, Buy, Reroll, Continue and Reroll icon c2, tier tab, sold panel and sold icon c1. The gunmetal and teal look without white plates (rule A6) is accepted on purpose.

## Method
- The owner retouches UMG by hand. A `.uasset` that is modified and uncommitted in git holds his edits: commit it alone as found, never restore or overwrite it.
- Rule [[umg-layout]]: Canvas and anchors first, no fixed pixel sizes except native-size images, no Row or Column to centre on a font, few nesting levels, clear names, the doc lists the anchors.
- Captures run at 1920x1080, 1280x720, 1920x1200 and 2560x1080; the last two need `Tools/capture/capture.js --force-res` (the engine otherwise shrinks them to 1600x900).

## Open defects (no task)
- Burn glitch at 1.10 s: "NO REFUNDS." shows over "SOLD" with no wrench.
- Legendary tab dimmed and hard to read at 1280x720 when unaffordable.
- Wide empty gaps between cards at 2560x1080.
- A 4-digit balance reaches the right edge of the ore counter plate.
- BUY text off-centre and BUY plate shrunk since 353; indicator icon and text 1 to 3 px high.
- Ore icon size follows the font by hand (ratio 60/44), not by binding.
- `MI_CardBurn` flames are fixed yellow, not tier-coloured (material change); the Buy flash is a square that exceeds the plate.

## Not verified on screen
- Hover and pressed tints were never seen: the simulated clicks of the task-355 capture went to another program (Romancing SaGa 2) on the main screen. Ask the owner to check that program for stray input; redo with the game window on the main screen. The Buy hover tint 1.35 may be capped.
- `TierTabText` size 20 not read back; "NEED 100 MORE" never captured (no debug command forces a tier); real mouse clicks never tested.
- Failing tests, out of scope: `Integration.LevelUpUpgradeLoop` (RerollButton enabled) and `Integration.JuiceKillDropsStreamingCubes` fail without these changes too; `Text.AlertEventsWired` also fails.

## Doc debt and cost
- `wbp-shop-screen` and `wbp-shop-card` exceed 400 words; their `verified_*` lines are stale for 351 to 353; `--force-res` is only noted in `verified_anchors`.
- Several subtasks went over their cost ceiling; "resumed N times" alerts were judged spurious.

## Repo state at writing
- `Content/RogueRigs/UI/WBP_ShopScreen.uasset` and `WBP_ShopCard.uasset` show as modified and uncommitted, after the last task commits. Cause unknown (owner edit or editor save): inspect, do not discard.
- Left uncommitted on purpose: `vault/scripts/usage-snapshot.json`, `Tools/content/logs/20261028-170000-screen_texts.review.json`, untracked `assignments/A7-complete-ai-dev-pipeline/`, the raw Kie.ai outputs in `Art/Source/shop-screen/` (`*-shop-screen*.png`, `.json`, `.review.json`) and the local captures under `Saved/`. Proposal for the owner: commit the raw generations or git-ignore them, per the "save art files in vault" rule.

## How to resume
Open [[umg-layout]] and `wbp-shop-screen`, then ask the owner with `/task` for a fixes task from the defects above; start by checking what changed in the two modified `.uasset` files.

## Relations
- [[production-index]] — parent index.
- [[tasks-index]] — task status source of truth.
- [[umg-layout]] — layout rule used from task 356.
- [[handover-2026-10-21-lighting-floor]] — previous handover.
