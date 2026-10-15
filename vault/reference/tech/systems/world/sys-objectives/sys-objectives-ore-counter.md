---
id: sys-objectives-ore-counter
description: "Ore counter as built: carried ore in the run flow, secured ore in a game-instance bank, extraction, loss, OnOreChanged. Open to change ore counting."
parent: sys-objectives
kind: doc
---
# Objectives — ore counter

- responsibility: count the ore the miner carries in a level and the ore secured this session ([[ore-economy]], [[extraction]]); task-183-01.
- main_classes:
  - `URRRunFlowSubsystem` (`Objectives/RRRunFlowSubsystem.h`) owns the carried count of its world: `AddCarriedOre(Amount)` (> 0; refused once the run ended or the miner died); `EndRun(true)` calls `SecureCarriedOre` (carried added to the secured total, carried 0); `EndRun(false)` and the `OnDeath` of a registered miner (bound in `RegisterMiner`) call `LoseCarriedOre` (carried 0, secured unchanged). Both run before `OnRunEnded`, and close the counter for that world.
  - `URROreBankSubsystem` (`Objectives/RROreBankSubsystem.h`, `UGameInstanceSubsystem`): `SecuredOre` of the session, survives `RestartLevel` (OpenLevel keeps the game instance), not saved to disk. `Get(WorldContext)`, `GetSecuredOre`, `AddSecuredOre`. A world without an initialized game instance (bare test worlds) uses the run flow's `LocalSecuredOre` instead.
- entry_points: `ARRPickup::Collect` of an Ore pickup calls `AddCarriedOre(OreValue)` ([[sys-pickups]]); the `GrantOre` cheat ([[sys-cheats]]). Read: pure `GetCarriedOre`, `GetSecuredOre` (category `RunFlow|Ore`). Event: `OnOreChanged(int32 CarriedOre, int32 SecuredOre)` (`FRROnOreChangedSignature`, BlueprintAssignable), sent on every change of either count, never without a change ([[sys-hud-events]]).
- data: none; ore value per chunk from `OreTypes.csv` `ChunkValue` (1).
- depends_on: [[sys-pickups]], [[sys-player]] (`OnDeath`).
- implements: [[ore-economy]] (carried, secured, lost on a miss); [[extraction]] (ore secured on boarding).
- gotchas: the share of secured ore lost on a failed run is `open` ([[ore-economy]]): not built, nothing removes secured ore. Any registered miner's death empties the carried count (single player). Log lines: `URRRunFlowSubsystem: extraction secured N ore, secured M.`, `...: shuttle missed|miner died, N carried ore lost.` No dedicated automation test (owner rule); proven by a `-game` run of `rr.Cheat.GrantOre 10` (carried 10).

## Relations
- [[sys-objectives]] — parent hub: summary and verdict.
- [[ore-economy]] — the rules counted here.
- [[extraction]] — the moment ore is secured.
- [[sys-pickups]] — the ore pickups that feed the count.
- [[sys-hud-events]] — the panel event pattern OnOreChanged follows.
- [[sys-cheats]] — the GrantOre cheat.
