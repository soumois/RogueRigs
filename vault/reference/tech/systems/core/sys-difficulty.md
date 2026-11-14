---
id: sys-difficulty
description: "Difficulty as built: alert and threat counters per level, URRGE_EnemyDifficulty applied at spawn from a table indexed by run level and threat."
parent: systems-core-index
kind: doc
---
# Difficulty

- responsibility: level counters and enemy strength at spawn.
- main_classes: `URRGE_EnemyDifficulty`; alert counter owner: `URRRunFlowSubsystem` ([[sys-objectives]]); threat counter: not built.

## Rules
- Alert: grows with time and kills (kills only once the first horde front is over, task-454-01, [[sys-objectives-run-flow]]), pauses while a mandatory objective is pending, fires tier events.
- Threat: grows with time and kills, never pauses.
- At spawn, `URRGE_EnemyDifficulty` applies `MultiplyAdditive` bonuses on `MaxHealth` and `AttackPower`; magnitudes come from a table indexed by (run level, threat). Robots already alive are not updated.
- Enemy attack damage = `AttackDamage` (CSV) × `AttackPower`.
- Spawn rate also follows threat.

- Horde kill gauge (task-544, commit 5efe2056; [[level-flow]] M1): `FRRWaveDirectorConfig::HordeTrigger` = `KillGauge` by default, data `FRRDirectorKillGauge` on the spawner's DirectorConfig: `StartConsoleFraction` 0.5 (the single alert-bar "!" at half of the console mark), `FirstHordeKills` 70, `NextHordeKills` 150, `HordeEndSeconds` 30, `KillWeightSteps` 210 s x2 / 270 s x3, `OvertimeHealthStep` 0.2 (all provisional). The run flow forwards every kill (also while paused) to `ARREnemySpawner::NotifyRobotKilled`; a full gauge starts the next horde's lull (horde data = `Hordes[i]`, the last one beyond the list); it stays full, kills not counted, until the horde's robots are all gone or 30 s after its front, then empties. No fixed-time or 30 s overtime horde. Each horde triggered from `Overtime.StartTime` adds 0.2 to the health factor applied at delivery (`ARREnemy::ApplySpawnHealthMultiplier`, robots spawned afterwards only). Events: `ARREnemySpawner::OnHordeGaugeChanged(State, Fill)`, relayed by `URRRunFlowSubsystem::OnHordeGaugeChanged` (+ `GetHordeGaugeState`, `GetHordeGaugeFill`) and by the panel event `URRHUDPanelWidget::OnHordeGaugeChanged`. Debug: `rr.Debug.HordeGauge [Kills]`. Tests: `RogueRigs.Spawner.DirectorKillGauge`, `DirectorKillGaugeWorld`, `RogueRigs.HUD.PressureHordeGauge`. The RunTime and Alert triggers remain available (tests, stress).
- data: difficulty table CSV; alert gains M1 provisional (240 s fill, 0.5 s per kill, `FRRRunFlowConfig`); threat gains open.
- depends_on: [[sys-gas-core]].
- implements: [[difficulty]], [[level-flow]].
- gotchas: set `MaxHealth` before `Health` when initializing a spawned robot.

## Relations
- [[systems-index]] — parent index.
- [[sys-objectives]] — where the alert counter lives.
