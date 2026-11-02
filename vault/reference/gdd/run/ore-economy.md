---
id: ore-economy
description: "Ore rules: carried ore secured only by extraction, lost on a missed shuttle, kept across levels, reset at run end, meta-progression deferred."
parent: run-index
kind: spec
status: approved
---
# Ore economy

- Ore drops from ore blocks, whatever destroyed them ([[ore-blocks]]), and from bonus chests ([[objective-bonus-chest]]). Ore pickups use the `PickupRadius` stat. Each ore chunk is worth `ChunkValue` of `OreTypes.csv` row `Duskite` (owner 2026-10-10, provisional).
- Ore carried during a level is secured only by a successful extraction. Missing the shuttle loses what the miner carries. This gives the risk window its stake.
- Ore lasts one run (owner 2026-10-28, provisional, [[task-340-ore-run-carry]]): secured ore is never reset between levels, so it can be saved across levels for a big purchase; all ore is reset to 0 when the run ends (lost, won, restart, New Run). The in-level HUD counter shows the run total (carried + secured). Every ore number on screen is the ore the player holds (owner 2026-10-30, [[task-391-final-screens-count-level-ore]]): HUD carried + secured, stats screen total, shop balance and run-won line the secured balance, purchases and rerolls subtracted; no screen shows ore collected since the run start.
- Two shops (owner 2026-10-17): the [[run-shop]] between levels spends collected ore on run-only purchases and its reroll; the level-up reroll is also paid in ore (owner 2026-10-26, [[xp-level-up]]); the meta shop gives upgrades for every run: deferred (cut from the capstone), its currency open.
- Ore lost when a run fails: all of it, as at any run end (owner 2026-10-28, provisional; was open).
- Special ore is an objective resource, not currency ([[objective-variants]]).

## Relations
- [[run-index]] — parent index.
- [[extraction]] — the moment ore is secured.
- [[wishlist]] — the meta shop.
- [[run-shop]] — the in-run shop paid in ore.
- [[xp-level-up]] — the level-up reroll paid in ore.
