---
id: sys-spawner-director
description: "Wave director of the spawner as built: data (phases, one-type packs, hordes), scheduler, horde lull/banner/front, delegates for the HUD; open to tune pacing."
parent: sys-spawner
kind: doc
---
# Spawner wave director

- responsibility: when and what the spawner spawns while `DirectorConfig.bEnabled` (default): calm, lone robots, waves of one-type packs from distinct sides, announced hordes ([[enemies-index]] wave director, [[drgs-analysis]]).
- main_classes: `Source/RogueRigs/Enemies/RRWaveDirector.h/.cpp`: data structs `FRRWaveDirectorConfig` {`bEnabled`, `HordeTrigger` (`ERRHordeTrigger` RunTime | Alert), `Phases`, `PackTypes`, `Hordes`}, `FRRDirectorPhase` {`Name`, `StartTime`, `SingleInterval` (first one at phase start), `SingleClass`, `bWaveOnStart`, `WaveIntervalStart` → `WaveIntervalEnd` over `WaveIntervalRampSeconds`, `SidesMin`–`SidesMax` packs per wave, `PackWeights` {`PackType` name, `Weight`}}, `FRRDirectorPackType` {`Name`, `Members`}, `FRRDirectorPackMember` {`RobotClass`, `CountMin`, `CountMax`, `bFront`}, `FRRDirectorHorde` {`TriggerTime` (front time), `TriggerAlert` (lull start), `LullSeconds`, `WarningSeconds`, `SpreadSeconds`, `SizeMin`, `SizeMax`, `Members`, `FillClass`}; world-free scheduler `FRRWaveDirector` (`Advance` → `FRRDirectorAction` orders). Executed by `ARREnemySpawner` ([[sys-spawner]]).
- defaults (`FRRWaveDirectorConfig::MakeDefault`, provisional, owner 2026-10-05): Calm 0 s; Isolated 10 s, a chaser every 3 s (10, 13, 16, 19); FirstPack 22 s, one `FirstChasers` pack (chaser 8–10); Flow 28 s, waves of 2–3 packs, interval 6 → 3 s over 180 s, weights Chasers 60 / Shooters 25 / Heavies 15. Pack types: Chasers = chaser 8–15 + kamikaze 2–3 mixed; Shooters = shooter 3–5; Heavies = heavy 2–3 front + chaser 8–12. Hordes: fronts at 95 s and 175 s (alert mode: 0.4 and 0.75), lull 5 s, banner 4 s before, 40–60 robots over 2 s: heavy 3–5 front, kamikaze 3–5, chasers fill. Choices of task-026-01: Heavies pack escort, horde composition, Flow weights.
- behavior: lull, banner and front silence lone robots and waves; the wave timer restarts after the front. Robot classes resolve to the `WeightedEnemyClasses` entry with the same nearest native class (the Blueprints of [[bp-enemy-spawner]]), else the class itself; entry `MaxAlive` caps apply (excess dropped; horde fill keeps the size). Pack: one candidate scan per wave, a distinct side per pack (random side with room, else largest), the cells nearest a random anchor, front members on the cells nearest the view. Horde: one side, cells sorted by distance to the view (the front row spans the side), front members first, spawned in order over `SpreadSeconds`; a cell that entered the margin meanwhile is skipped. Packs and hordes clamp to `MaxAlive` (150).
- entry_points: delegates `OnDirectorPhaseChanged(PhaseIndex, PhaseName)`, `OnHordeLull(HordeIndex)`, `OnHordeIncoming(HordeIndex, SecondsUntil)` (banner), `OnHordeStarted(HordeIndex, Side)`, `OnHordeFinished(HordeIndex, Spawned)`; pure `IsDirectorEnabled`, `GetDirectorPhase`, `GetHordeTrigger`, `GetHordeCount`, `GetHordeTriggerTime(i)`, `GetHordeTriggerAlert(i)`, `GetHordeState(i)` (`ERRHordeState`), `GetActiveHorde`, `GetSecondsUntilHordeFront`; C++ `GetDirectorConfig`, `GetDirector`, `ResolveRobotClass`, `TrySpawnDirectorWave` (→ `FRRSpawnedPack` {type, side, robots, cells}), `TrySpawnEnemyOfClass`.
- data: `ARREnemySpawner::DirectorConfig` (EditAnywhere, category Spawner|Director); tune on [[bp-enemy-spawner]] or the C++ defaults.
- depends_on: [[sys-spawner]], [[sys-objectives]] (alert, Alert trigger only).
- implements: [[enemies-index]] wave director; horde events for [[level-flow]] M1.
- gotchas: run time counts only enabled spawning. Alert mode at 0.4 coincides with the console drop. A saturated cap shrinks hordes. Heavy capsules nudge neighbours off compact pack cells. Tests: `RogueRigs.Spawner.Director*` ([[sys-automation-tests]]).

## Relations
- [[sys-spawner]] — parent: the spawner that executes the orders.
- [[enemies-index]] — the provisional wave rule.
- [[sys-automation-tests]] — `Spawner.Director*` tests.
