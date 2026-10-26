---
id: run-shop
description: "Shop between levels of a run: 5 upgrade cards priced by rarity, paid in ore, escalating reroll; purchases valid for the current run only."
parent: run-index
kind: spec
status: provisional
---
# Run shop

Owner 2026-10-17, provisional; catalogue, prices and reroll decided by the owner 2026-10-26 ([[task-316-intermission-shop]]), provisional.

- Opens on the [[intermission]] screen between two levels. Buying is optional.
- Purchases last for the current run only. Upgrades that apply to every run belong to the meta shop ([[ore-economy]]), a separate system.
- Currency: the ore collected during the run (the current ore counter; at the intermission all of it is secured).
- Catalogue: 5 upgrade cards at once, drawn like a level-up offer with the miner's luck ([[rarities]]), stat cards only (no new-weapon card).
- Prices by rarity: grey 25, green 40, blue 60, purple 80, yellow 100 (the owner's "blanche, verte, bleue, épique, légendaire").
- Several cards may be bought; a bought card applies at once, carries into the next levels like a picked card ([[run-structure]]) and leaves its slot empty. A card the miner cannot pay is greyed.
- Reroll: redraws the 5 slots; same cost rule as the level-up reroll ([[xp-level-up]]): 15, then +5 per further reroll; its own count, back to 15 at each intermission.
- Layout: the level-up screen keeps its 3 cards; the shop shows 5 smaller cards in one row, same card look, without the deal, suspense or fly animations; the recap stays at the top, the ore balance is shown, then Reroll and Continue.

## Relations
- [[run-index]] — parent index.
- [[intermission]] — the screen that hosts it.
- [[ore-economy]] — ore rules and the meta shop.
- [[xp-level-up]] — the reroll cost rule it shares.
- [[rarities]] — the tiers that set the prices.
