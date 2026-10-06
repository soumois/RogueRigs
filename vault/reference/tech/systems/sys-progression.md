---
id: sys-progression
description: "XP and level as built: URRProgressionComponent, XPCurve.csv (5, then 12 × (level + 3)) and its measured pacing, XPGain, carry-over, OnXPChanged and OnLevelUp."
parent: systems-index
kind: doc
---
# Progression

- responsibility: the miner's XP and level: accumulate XP, level up along the CSV curve with carry-over, broadcast events for the HUD. What a level-up triggers (heal, effect, paused upgrade choice) is the level-up flow, `URRLevelUpComponent` in the same folder ([[sys-level-up]]).
- main_classes (`Source/RogueRigs/Progression/`): `URRProgressionComponent : UActorComponent` (`RRProgressionComponent.h`, no tick), a default subobject `Progression` of `ARRMiner` (`GetProgression()`); `FRRXPCurveRow` (`RRXPCurveRow.h`, `FTableRowBase`, one float `XPToNextLevel`).
- entry_points:
  - `AddXP(float Raw)` (BlueprintCallable): `Raw` <= 0 ignored; adds `Raw × XPGain` (owner ASC `URRPlayerAttributeSet::XPGain`, 1 without the set); while XP >= the need of the current level - 0.001 (fractional XP cubes, task-027-01): subtract it (floored at 0), level + 1, `OnLevelUp(NewLevel)`; then `OnXPChanged(Current, Needed, Level)` once. Several levels can pass in one call.
  - Getters (BlueprintPure): `GetLevel()` (starts at 1), `GetXP()` (XP inside the level, starts at 0), `GetXPToNextLevel()`; native `GetXPToNextLevelAt(Level)`.
  - Delegates (BlueprintAssignable): `OnXPChanged(float Current, float Needed, int32 Level)`, `OnLevelUp(int32 NewLevel)` (once per level gained, before `OnXPChanged`).
  - Callers: XP pickups on collection ([[sys-pickups]]); the HUD ([[sys-hud]]) and the level-up flow ([[sys-level-up]]) listen to `OnLevelUp`.
- data: `Content/RogueRigs/Data/Tables/XPCurve.csv`, columns `Name,XPToNextLevel`, one row per level, row name = level, levels 1 to 50: need(1) = 5, need(L ≥ 2) = 12 × (L + 3) (60, 72, 84 … 636; task-028-03, tuned on the pacing bot, provisional, [[xp-level-up]]; was 5 × L). Read with `FRRBaseStatsLoader::GetCachedTable` in `InitializeComponent` (before any `BeginPlay`, so the HUD bind sees the need) and copied to an array. Levels above the last row reuse its value. Rows must run 1..N without gap with a need > 0, otherwise the curve is rejected (error logged, need 0, no level-up). Staged with the other tables ([[sys-gas-core]]).
- depends_on: [[sys-gas-core]] (XPGain, CSV loader), [[sys-player]] (owner).
- implements: [[xp-level-up]] (XP bar data, level-up), [[player-stats]] (XPGain).
- gotchas: XP is a float (XPGain can be fractional); the comparison is exact, so a curve with fractional needs may leave XP a hair below a level. XP is not reset on death (the level restarts). Tests: `RogueRigs.Progression.*` (level-up and weapons integration tests compute their XP from the curve rows since task-028-03) ([[sys-automation-tests]]).

## Pacing (task-028-03)
- Bot: `rr.Pacing.Run [Seconds=200] [Seed=1] [Quit=1] [Collect=1]` (`Debug/RRPacingBotSubsystem`, not in shipping): -game on L_ArenaM1, real wave director and pickup motion; seeds the placed spawner and the global random, miner invulnerable (cheat count), stick aimed at the nearest live robot each frame, walks to the nearest XP cube lying outside 0.8 × pickup radius within `rr.Pacing.CollectRange` (2500 uu), picks card 0 of each screen. Clock = spawner `GetRunTime`. Logs `Pacing: level` and `Pacing: status` (every `rr.Pacing.StatusInterval`, 15 s) lines, writes `Saved/Pacing/*.csv`. Same seed does not give the same run (frame times differ).
- Results, level time in s (target ±20 %: L2 24–36, L5 80–120, L10 152–228):

| Curve | Seeds | L2 | L5 | L10 |
|---|---|---|---|---|
| 5 × L (before) | 1 | 28.3 | 57.4 fail | 108.0 fail |
| 5, then 5 × (L + 8) | 1 | 26.8 | 86.8 | 151.5 fail |
| 5, then 10 × (L + 3) | 1, 2 | 26.8, 30.9 | 88.9, 76.8 | 174.7, 150.1 |
| 5, then 12 × (L + 3) (kept) | 1, 2, 3 | 26.8, 30.8, 26.0 | 100.7, 80.2, 93.0 | ≈ 201, 164.7, 190.0 |

- Kept curve, median over seeds 1–3: L2 26.8 (−11 %), L5 93.0 (−7 %), L10 190.0 (0 %); every seed within ±20 % (seed 2 L5 −19.8 %). Seed 1 ended at L9 with 138.9 / 144 XP at 200 s. Variance between seeds ≈ ±10 %: pick order (card 0) and pack sides change the kill rate.
- The baseline run (5 × L) still had the pickup collision bug ([[sys-pickups]]): its miner sometimes stuck on cubes.

## Relations
- [[systems-index]] — parent index.
- [[xp-level-up]] — gameplay spec it implements.
- [[sys-player]] — the miner that owns the component.
- [[sys-pickups]] — XP pickups that call AddXP.
- [[sys-hud]] — the widget fed by its events.
- [[sys-level-up]] — the flow run on each level gained.
- [[sys-cheats]] — rr.Cheat.GrantXP calls AddXP for PIE checks.
- [[task-028-03-xp-pacing]] — the measured pacing run that set the curve.
