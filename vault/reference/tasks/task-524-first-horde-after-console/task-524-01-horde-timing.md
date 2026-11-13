---
id: task-524-01-horde-timing
description: "Cpp: first horde front timed from the console landing per Q10"
parent: task-524-first-horde-after-console
kind: task
status: done
---
# Première horde un peu après l'arrivée de la console: cpp

- goal: first horde front timed from the console landing per Q10
- agent: cpp
- depends_on: none
- inputs: enemies-spawn-rules (vault/reference/gdd/enemies/enemies-spawn-rules.md), level-flow (vault/reference/gdd/run/level-flow.md), task-454-swarm-icons-fixed (vault/reference/tasks/task-454-swarm-icons-fixed.md)
- expected_output: see done_when
- done_when: A1, A2, A4 (code part) hold; commit.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-524
- usage: included in the executor run of task-524
- runs: 1
- milestone: M2
- started: 2026-11-13T13:41:04Z
- waiting_for: none
- result: Done (f2d936af). FRRDirectorHorde::ConsoleLandedDelaySeconds (MakeDefault: 20 s on horde 0, -1 elsewhere and on the overtime copy); FRRWaveDirector SetConsoleExpected / NotifyConsoleLanded / GetHordeFrontTime: the spawner sets the console expected while the run flow is enabled and notifies the landing from URRRunFlowSubsystem::GetConsole; the console-timed horde waits until then. Run flow alert bar: a console-timed horde icon fixed at ConsoleAlertThreshold + delay / AlertFillSeconds (0.483), pending or done. Banner now logged. A1 -game log (L_ArenaM1, pacing bot): console landed 96.4 s, horde 0 lull 111.7, banner 112.6 (front in 3.8 s), front 116.5 s. A2 horde 1 banner 171.1, front 175.1 s. A4 build green; RogueRigs.Spawner + HUD.Pressure + Integration.Pressure + RunFlow 31/31 passed, new test Spawner.DirectorConsoleTimedHorde (landing 96 s: banner 112.01, front 116.01, front 1 175.00); expectations unchanged, setups that force horde 0 TriggerTime set ConsoleLandedDelaySeconds -1 (HUD.PressureHorde, HUD.PressureKillsWaitFirstHorde, ShortHordeConfig, Integration.PressureHordeBanner) and the stress forced horde too. Build fix outside scope: HEAD did not compile (two DEFINE_LOG_CATEGORY_STATIC(LogRRAudio) in one unity file, RRDistanceSound.cpp of task-522 and RRMusicSubsystem.cpp); RRMusicSubsystem.cpp now uses its own C++ category LogRRMusic with the same runtime name LogRRAudio; task-522 files untouched. ALERT at closing: executor run 77 turns, 1.51 M units, over the standard budget (1.5 M); cause: the unity-build failure from task-522 code to diagnose and fix, plus two 2-4 min -game runs (the first ended before the banner).

## Relations
- [[task-524-first-horde-after-console]] — parent global task.
