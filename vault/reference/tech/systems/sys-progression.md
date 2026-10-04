---
id: sys-progression
description: "XP and level as built: URRProgressionComponent on the miner, XPCurve.csv (5 × level), XPGain scaling, carry-over, OnXPChanged and OnLevelUp events."
parent: systems-index
kind: doc
---
# Progression

- responsibility: the miner's XP and level: accumulate XP, level up along the CSV curve with carry-over, broadcast events for the HUD. What a level-up triggers (heal, effect, paused upgrade choice) is the level-up flow, `URRLevelUpComponent` in the same folder ([[sys-level-up]]).
- main_classes (`Source/RogueRigs/Progression/`): `URRProgressionComponent : UActorComponent` (`RRProgressionComponent.h`, no tick), a default subobject `Progression` of `ARRMiner` (`GetProgression()`); `FRRXPCurveRow` (`RRXPCurveRow.h`, `FTableRowBase`, one float `XPToNextLevel`).
- entry_points:
  - `AddXP(float Raw)` (BlueprintCallable): `Raw` <= 0 ignored; adds `Raw × XPGain` (owner ASC `URRPlayerAttributeSet::XPGain`, 1 without the set); while XP >= the need of the current level: subtract it, level + 1, `OnLevelUp(NewLevel)`; then `OnXPChanged(Current, Needed, Level)` once. Several levels can pass in one call.
  - Getters (BlueprintPure): `GetLevel()` (starts at 1), `GetXP()` (XP inside the level, starts at 0), `GetXPToNextLevel()`; native `GetXPToNextLevelAt(Level)`.
  - Delegates (BlueprintAssignable): `OnXPChanged(float Current, float Needed, int32 Level)`, `OnLevelUp(int32 NewLevel)` (once per level gained, before `OnXPChanged`).
  - Callers: XP pickups on collection ([[sys-pickups]]); the HUD ([[sys-hud]]) and the level-up flow ([[sys-level-up]]) listen to `OnLevelUp`.
- data: `Content/RogueRigs/Data/Tables/XPCurve.csv`, columns `Name,XPToNextLevel`, one row per level, row name = level, levels 1 to 50 with 5 × level (5, 10 … 250; curve provisional, [[xp-level-up]]). Read with `FRRBaseStatsLoader::GetCachedTable` in `InitializeComponent` (before any `BeginPlay`, so the HUD bind sees the need) and copied to an array. Levels above the last row reuse its value. Rows must run 1..N without gap with a need > 0, otherwise the curve is rejected (error logged, need 0, no level-up). Staged with the other tables ([[sys-gas-core]]).
- depends_on: [[sys-gas-core]] (XPGain, CSV loader), [[sys-player]] (owner).
- implements: [[xp-level-up]] (XP bar data, level-up), [[player-stats]] (XPGain).
- gotchas: XP is a float (XPGain can be fractional); the comparison is exact, so a curve with fractional needs may leave XP a hair below a level. XP is not reset on death (the level restarts). Tests: `RogueRigs.Progression.*` ([[sys-automation-tests]]).

## Relations
- [[systems-index]] — parent index.
- [[xp-level-up]] — gameplay spec it implements.
- [[sys-player]] — the miner that owns the component.
- [[sys-pickups]] — XP pickups that call AddXP.
- [[sys-hud]] — the widget fed by its events.
- [[sys-level-up]] — the flow run on each level gained.
- [[sys-cheats]] — rr.Cheat.GrantXP calls AddXP for PIE checks.
