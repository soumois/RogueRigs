---
id: task-544-01-gauge-logic
description: "Cpp: Single "!" at half of C, kill gauge state and horde trigger replacing the fixed horde times"
parent: task-544-horde-kill-gauge
kind: task
status: done
---
# Hordes: one "!" at half of C, then a kill gauge under the alert bar triggers each horde: cpp

- goal: Single "!" at half of C, kill gauge state and horde trigger replacing the fixed horde times
- agent: cpp
- depends_on: none
- inputs: level-flow (vault/reference/gdd/run/level-flow.md), enemies-spawn-rules (vault/reference/gdd/enemies/enemies-spawn-rules.md), difficulty (vault/reference/gdd/run/difficulty.md), sys-difficulty (vault/reference/tech/systems/core/sys-difficulty.md)
- expected_output: see done_when
- done_when: A1 to A6 and A8 hold in code and tests (overtime: kill weight x2 from 3:30, x3 from 4:30, no 30 s horde timer, +20 % base health per overtime horde for robots spawned after it) (gauge state exposed by events for the widget); kill counts in data.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-544
- usage: included in the executor run of task-544
- runs: 1
- milestone: M2
- started: 2026-11-14T00:16:24Z
- waiting_for: none
- result: Commit 5efe2056: ERRHordeTrigger::KillGauge is the default (FRRDirectorKillGauge: StartConsoleFraction 0.5, 70 then 150 kills, HordeEndSeconds 30, KillWeightSteps 210 s x2 / 270 s x3, OvertimeHealthStep 0.2); no fixed-time or 30 s overtime horde; ARREnemySpawner OnHordeGaugeChanged, NotifyRobotKilled, GetHordeGauge* getters, horde robots tracked for the clear; URRRunFlowSubsystem forwards every kill (also while paused), relays OnHordeGaugeChanged, GetHordeGaugeState/Fill, one alert-bar Horde mark at the gauge start until it shows; ARREnemy::ApplySpawnHealthMultiplier; rr.Debug.HordeGauge [Kills]. Tests RogueRigs.Spawner + HUD.Pressure + Integration.Pressure + RunFlow: 34 pass (new DirectorKillGauge, DirectorKillGaugeWorld, HUD.PressureHordeGauge; PhaseTimes, Overtime, ConsoleTimedHorde set the RunTime trigger explicitly).

## Relations
- [[task-544-horde-kill-gauge]] — parent global task.
