---
id: run-shop
description: "Shop between levels of a run: 5 upgrade cards priced by rarity, paid in ore, escalating reroll; purchases valid for the current run only."
parent: run-index
kind: spec
status: provisional
---
# Run shop

Owner 2026-10-17, provisional; catalogue, prices and reroll decided by the owner 2026-10-26 ([[task-316-intermission-shop]]), provisional.

- Opens on the cards screen of the [[intermission]], between two levels only (not after level 4, owner 2026-10-26). Buying is optional.
- Purchases last for the current run only. Upgrades that apply to every run belong to the meta shop ([[ore-economy]]), a separate system.
- Currency: the ore collected during the run (the current ore counter; at the intermission all of it is secured).
- Catalogue: 5 upgrade cards at once, drawn like a level-up offer with the miner's luck ([[rarities]]), stat cards only (no new-weapon card).
- Prices by rarity: grey 25, green 40, blue 60, purple 80, yellow 100 (the owner's "blanche, verte, bleue, épique, légendaire").
- Several cards may be bought; a bought card applies at once, carries into the next levels like a picked card ([[run-structure]]) and leaves its slot empty. A card the miner cannot pay is greyed.
- Reroll: redraws the 5 slots; same cost rule as the level-up reroll ([[xp-level-up]]): 15, then +5 per further reroll; its own count, back to 15 at each intermission.
- Layout: the level-up screen keeps its 3 cards; the shop shows 5 smaller cards in one row, same card look, without the deal, suspense or fly animations; on its own screen over the finished level, behind a full-screen black at 70 % opacity, the same as the level-up cards screen (owner 2026-10-30, [[task-398-upgrade-screen-backdrop]]; was an opaque black background; the recap is on the stats screen, owner 2026-10-26), the ore balance is shown, then Reroll and Continue.
- Purchase effects (owner 2026-10-28, values provisional, [[task-342-shop-ore-flight]], [[task-343-shop-buy-juice]], [[task-344-shop-card-burn]]): on a buy, the Buy button punches, flashes and throws tier-coloured sparks; ore icons (one per 10 of price, 3 to 10) fly from the balance to that button while the balance counts down; when the last lands, the bought card burns away tinted by its tier, then shows "Sold"; the level-up flight to a weapon or the XP bar is not used; every shop button is blocked during the sequence (about 1.2 s). Design and game feel of the screen are under discussion ([[task-341-shop-design-prompt]]).
- Screen look (owner 2026-10-28, provisional, after the ChatGPT mockup [[art-prompts-shop-screen-mockup]], [[task-348-shop-screen-redesign]]): title "SPARE PARTS" with side bars and subtitle "RIG SERVICE TERMINAL"; ore counter in a pipe-framed panel at the top left; the 5 cards in a riveted teal and rust panel; under it Reroll ("REROLL", arrow, cost), then Continue. Cards keep the current tier frames, stat icons and target line; a tier tab names the tier in its colour (COMMON grey, UNCOMMON green, RARE blue, EPIC purple, LEGENDARY yellow); price always ore violet (0.85, 0.45, 1.0); a card the miner cannot pay is dimmed with a grey "NEED {n} MORE" button (n = price minus balance); labels in upper case ("BUY", "CONTINUE"); sold slot: burnt panel in the grey frame, wrench icon, "SOLD", "NO REFUNDS.". Font Roboto Bold; title is text. Rules unchanged.
- Panel frame and layout (owner 2026-11-13, UI batch, provisional, owner target image 8; replaces the card size 330 x 470 below where they differ): the card panel gets its own frame image made for its size and ratio (was the button plate frame, stretched), 9-slice with corners, bolts and side pieces unstretched; inner background darker and uniform, cards with a margin to the frame, proportions as image 8.
- Screen fit at 1920 x 1080 (owner 2026-10-28, provisional, "OK, tout par défaut" to the fixes of the task-348 capture, [[task-351-shop-title-clip]] to [[task-355-shop-card-frame-details]]): content anchored to the top with 16 px above the title bars; Reroll and Continue plates drawn inside their boxes, Continue ending at least 16 px above the screen bottom; cards keep 330 x 470; ore counter top left, scaled so its bottom stays at least 8 px above the shop panel, which does not move; "NEED {n} MORE" in Bold 20 (BUY 28) on the Buy plate greyed by its disabled tint, no new sprite, fitting "NEED 100 MORE"; tier tab text Bold 20; sold panel fitted inside the card frame opening; no second frame line beside the first slot.
- Screen adaptation (owner 2026-10-28, provisional, "OK, tout par défaut", [[task-356-shop-screen-anchors]], [[task-357-shop-resource-indicator]]): the screen is laid out with anchors so the fit above holds at 1920 x 1080, 1280 x 720, 1920 x 1200 and 2560 x 1080 (pixel values are the 1920 x 1080 reference); the cards keep their ratio and fill their area; in the ore counter the icon and the value sit on the same centre line, the icon sized from the font, not moving when the number of digits changes.

## Relations
- [[run-index]] — parent index.
- [[intermission]] — the screen that hosts it.
- [[ore-economy]] — ore rules and the meta shop.
- [[xp-level-up]] — the reroll cost rule it shares.
- [[rarities]] — the tiers that set the prices.
