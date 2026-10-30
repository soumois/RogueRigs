---
id: sys-objectives-ore-counter
description: "Ore counter as built: carried ore in the run flow, secured ore in a game-instance bank, extraction, loss, spending, OnOreChanged. Open to change ore counting."
parent: sys-objectives
kind: doc
---
# Objectives — ore counter

- responsibility: count the ore the miner carries in a level and the ore secured this session ([[ore-economy]], [[extraction]]); task-183-01.
- main_classes:
  - `URRRunFlowSubsystem` (`Objectives/RRRunFlowSubsystem.h`) owns the carried count of its world: `AddCarriedOre(Amount)` (> 0; refused once the run ended or the miner died); `EndRun(true)` calls `SecureCarriedOre` (carried added to the secured total, carried 0, and to `ExtractionSecuredOre`, read by the C++-only `GetExtractionSecuredOre()` for the recap's data and logs, cleared by `ResetForNextLevel`, task-391-04); `EndRun(false)` and the `OnDeath` of a registered miner (bound in `RegisterMiner`) call `LoseCarriedOre` (carried 0, secured unchanged). Both run before `OnRunEnded`, and close the counter for that world.
  - `URROreBankSubsystem` (`Objectives/RROreBankSubsystem.h`, `UGameInstanceSubsystem`): `SecuredOre` of the run, survives `OpenLevel` between levels (the game instance stays), not saved to disk. `ResetSecuredOre()` (task-340-01) sets the total to 0 and returns the amount cleared; its only caller is `URRRunSubsystem::ResetRun` ([[sys-run]]), so ore lasts one run. It does not broadcast `OnOreChanged`. The bank keeps no since-reset total. `Get(WorldContext)`, `GetSecuredOre`, `AddSecuredOre`, `RemoveSecuredOre(Amount)` (clamped to the total, returns it). A world without an initialized game instance (bare test worlds) uses the run flow's `LocalSecuredOre` instead.
- entry_points: `ARRPickup::Collect` of an Ore pickup calls `AddCarriedOre(OreValue)` ([[sys-pickups]]); the `GrantOre` cheat ([[sys-cheats]]). Read: pure `GetCarriedOre`, `GetSecuredOre`, `GetSpendableOre` (carried + secured), `CanSpendOre(Amount)` (category `RunFlow|Ore`). Spend (task-315-01): `SpendOre(Amount)` (C++, bool) takes carried ore first, then the rest from the secured ore (bank, or `LocalSecuredOre`), one `OnOreChanged`, log `URRRunFlowSubsystem: N ore spent (c carried, s secured), carried C, secured S.`; refused without spending when Amount <= 0 or > spendable. Works after the run ended too (carried is 0, secured pays), for the task-316 shop. Callers: the paid level-up reroll ([[sys-level-up]]); the intermission shop buy and reroll ([[sys-run-shop]]). Event: `OnOreChanged(int32 CarriedOre, int32 SecuredOre)` (`FRROnOreChangedSignature`, BlueprintAssignable), sent on every change of either count, never without a change ([[sys-hud-events]]).
- data: none; ore value per chunk from `OreTypes.csv` `ChunkValue` (1).
- depends_on: [[sys-pickups]], [[sys-player]] (`OnDeath`).
- implements: [[ore-economy]] (carried, secured, lost on a miss); [[extraction]] (ore secured on boarding).
- gotchas: every on-screen ore number (stats Total Ore, shop balance, run-won line, HUD) is the spendable balance held (owner 2026-10-30, task-391-04); a shop purchase or level-up reroll lowers the next stats figure. Verified in a -game run: stats totals 103, 68, 75, 86 equal the shop balances, run-won 86, reroll paid 15 lowered the next figures; map-travel path not run. All ore is reset to 0 when a run ends (loss, win, restart, New Run; decided by the owner 2026-10-28, [[ore-economy]]), by `ResetRun` and not in `BeginRun`; the bank otherwise only changes by extraction and spending. Log on reset: `URROreBankSubsystem: run ended, N secured ore cleared, session total 0.` Capture task-340-03 (-game): `GrantOre 7` on level 1, `EndLevel` secured 7, shop balance 7, level 2 HUD counter 7, `rr.Run.Restart` cleared 7 and the HUD showed 0; not exercised in the capture: loss, win and main menu New Run paths, mid-level pickups on level 2, shop buying. Any registered miner's death empties the carried count (single player). Log lines: `URRRunFlowSubsystem: extraction secured N ore, secured M.`, `...: shuttle missed|miner died, N carried ore lost.` No dedicated automation test (owner rule); proven by a `-game` run of `rr.Cheat.GrantOre 10` (carried 10). Spending order checked in `Integration.LevelUpUpgradeLoop` (10 carried + 20 secured, reroll 15: carried 0, secured 15); that test, `Integration.JuiceKillDropsStreamingCubes` and `Text.AlertEventsWired` fail for reasons unrelated to ore (known, pre-existing).

## Relations
- [[sys-objectives]] — parent hub: summary and verdict.
- [[ore-economy]] — the rules counted here.
- [[extraction]] — the moment ore is secured.
- [[sys-pickups]] — the ore pickups that feed the count.
- [[sys-hud-events]] — the panel event pattern OnOreChanged follows.
- [[sys-cheats]] — the GrantOre cheat.
- [[sys-level-up]] — the paid reroll that spends ore.
- [[sys-run-shop]] — the intermission shop that spends secured ore.
