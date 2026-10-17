---
id: sys-progression
description: "XP and level as built: URRProgressionComponent, XPCurve.csv (4, then reference curve / 5) and its measured pacing on two bot profiles, XPGain, carry-over, events."
parent: systems-core-index
kind: doc
---
# Progression

- responsibility: the miner's XP and level: accumulate XP, level up along the CSV curve with carry-over, broadcast events for the HUD. What a level-up triggers (heal, effect, paused upgrade choice) is the level-up flow, `URRLevelUpComponent` in the same folder ([[sys-level-up]]).
- main_classes (`Source/RogueRigs/Progression/`): `URRProgressionComponent : UActorComponent` (`RRProgressionComponent.h`, no tick), a default subobject `Progression` of `ARRMiner` (`GetProgression()`); `FRRXPCurveRow` (`RRXPCurveRow.h`, `FTableRowBase`, one float `XPToNextLevel`).
- entry_points:
  - `AddXP(float Raw)` (BlueprintCallable): `Raw` <= 0 ignored; adds `Raw × XPGain` (owner ASC `URRPlayerAttributeSet::XPGain`, 1 without the set); while XP >= the need of the current level - 0.001 (fractional XP cubes, task-027-01): subtract it (floored at 0), level + 1, `OnLevelUp(NewLevel)`; then `OnXPChanged(Current, Needed, Level)` once. Several levels can pass in one call.
  - Getters (BlueprintPure): `GetLevel()` (starts at 1), `GetXP()` (XP inside the level, starts at 0), `GetXPToNextLevel()`; native `GetXPToNextLevelAt(Level)`; native `RestoreProgress(Level, XP)` (run carry-over, [[sys-run]]: XP clamped to the need, no `OnLevelUp`, one `OnXPChanged`).
  - Delegates (BlueprintAssignable): `OnXPChanged(float Current, float Needed, int32 Level)`, `OnLevelUp(int32 NewLevel)` (once per level gained, before `OnXPChanged`).
  - Callers: XP pickups on collection ([[sys-pickups]]); the HUD ([[sys-hud]]) and the level-up flow ([[sys-level-up]]) listen to `OnLevelUp`.
- data: `Content/RogueRigs/Data/Tables/XPCurve.csv`, columns `Name,XPToNextLevel`, one row per level, row name = level, levels 1 to 50: 4 at level 1, then the reference curve ÷ 5 (20, 25, 31, 36 at level 5 … 64 at level 10 … 440 at level 50; owner 2026-10-06, [[xp-level-up]]; was L² from task-031-03, 12 then 12 × (L + 3) from task-030-02, 5 × L before). Not measured by the pacing bot: the table below is for older curves. Read with `FRRBaseStatsLoader::GetCachedTable` in `InitializeComponent` (before any `BeginPlay`, so the HUD bind sees the need) and copied to an array. Levels above the last row reuse its value. Rows must run 1..N without gap with a need > 0, otherwise the curve is rejected (error logged, need 0, no level-up). Staged with the other tables ([[sys-gas-core]]).
- depends_on: [[sys-gas-core]] (XPGain, CSV loader), [[sys-player]] (owner).
- implements: [[xp-level-up]] (XP bar data, level-up), [[player-stats]] (XPGain).
- gotchas: XP is a float (XPGain can be fractional); the comparison is exact, so a curve with fractional needs may leave XP a hair below a level. XP is not reset on death (the level restarts). Tests: `RogueRigs.Progression.*` (level-up and weapons integration tests compute their XP from the curve rows since task-028-03) ([[sys-automation-tests]]).

## Pacing bot
- `rr.Pacing.Run [Seconds=200] [Seed=1] [Quit=1] [Profile=1]` (`Debug/RRPacingBotSubsystem`, not in shipping; task-028-03, profiles task-031-03): -game on L_ArenaM1, real director and pickups; seeds the spawner and the global random; miner invulnerable; aims at the nearest live robot; picks card 0. Profile 0 still; 1 collect (walks to the nearest resting XP cube outside 0.8 × pickup radius within `rr.Pacing.CollectRange` 2500 uu); 3 run-and-collect (task-067-01: collect when such a cube exists, else run; drives the [[sys-stress]] game mode; with `rr.Pacing.RunLookahead 0` it turns only when stalled, so it pushes into walls and digs); 2 run (straight from screen north, turns when less than `rr.Pacing.RunLookahead` 200 uu of floor lies ahead or after moving under `rr.Pacing.RunStallDistance` 60 uu in 0.5 s, toward the freest of the 7 other 45° headings probed to `rr.Pacing.RunProbe` 3000 uu, going back counts half; collects only what the magnet takes). Clock = spawner `GetRunTime`. Logs `Pacing: level`, `Pacing: status` (every `rr.Pacing.StatusInterval` s: position, level, XP, robots, cubes, heading, turns) and the summary; writes `Saved/Pacing/Pacing_s<seed>_p<profile>_*.csv`. Same seed does not give the same run.

## Measured pacing (task-031-03)
Target: mean of the two profiles' medians at L2 15 s, L5 70 s, L10 180 s ± 20 %. Seeds 1–3, collect to 245 s, run to 300 s. Level time in s.

| Curve | Profile | L2 | L5 | L10 |
|---|---|---|---|---|
| 12, then 12 × (L + 3) | collect | 40.3, 42.7, 27.4 | 92.8, 93.9, 102.8 | 176.2, 183.1, 191.4 |
| 12, then 12 × (L + 3) | run | 26.2, 58.1, 55.8 | 163.4, 179.2, 155.2 | not reached at 245 s |
| L² (kept) | collect | 8.3, 5.2, 9.6 | 42.7, 45.4, 46.4 | 110.9, 105.5, 111.9 |
| L² (kept) | run | 80.0, 61.7, 63.6 | 103.2, 90.1, 71.1 | 215.8, 200.9, 143.3 |

- L² medians: collect 8.3 / 45.4 / 110.9, run 63.6 / 90.1 / 200.9; mean L2 36.0 (+140 %, fail), L5 67.8 (−3 %), L10 155.9 (−13 %).
- L2 cannot be reached by the curve: the runner's first collected XP comes at 19–80 s (it kills the chasers following it, so the cubes fall behind; dozens of cubes lie unclaimed). Even a need of 1 leaves the mean above 18 s.
- The profiles differ about 3.4 times in XP (median cumulative XP at 180 s, old curve: collect 810, run 235), so a curve set on their mean makes the collector fast (L10 at 1:51, level 16 at 4:00).
- Overtime (from 210 s, old-curve runs): no clear effect on XP within 210–240 s: collect 7.9–8.9 XP/s over 180–210 s, 8.1–9.7 over 210–240 s; runner unchanged (below 1 XP/s).
- Earlier results (task-028-03, task-030-02, single collect profile) are in git history of this node.

## Relations
- [[sys-run]] — restores level and XP between run levels.
- [[systems-index]] — parent index.
- [[xp-level-up]] — gameplay spec it implements.
- [[sys-player]] — the miner that owns the component.
- [[sys-pickups]] — XP pickups that call AddXP.
- [[sys-hud]] — the widget fed by its events.
- [[sys-level-up]] — the flow run on each level gained.
- [[sys-cheats]] — rr.Cheat.GrantXP calls AddXP for PIE checks.
- [[task-028-03-xp-pacing]] — the measured pacing run that set the curve.
- [[sys-stress]] — the real-game stress mode the bot drives.
