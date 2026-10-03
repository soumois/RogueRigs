---
id: task-013-01-baseline
description: "Stress command and -game frame-time measurement on the current build: S0 at 50 to 400 chasers; ceiling and optimization leads."
parent: task-013-stress-test
kind: task
status: in-progress
---
# Stress baseline

- goal: baseline frame-time table and ceiling for the current build.
- agent: cpp
- depends_on: none
- inputs: [[stress-test-m1]], [[perf-risks]], [[sys-spawner]], [[sys-enemies]], [[sys-enemy-ai]], [[build-toolchain]], [[cpp-standard]].
- expected_output: cheat console command `rr.Stress.Spawn <N> [Class]` (spawns N robots, chaser class by default, on reachable cells around the miner, ignoring MaxAlive; `rr.Stress.Clear` kills them) in C++; a launcher script under `Scripts/` that runs `UnrealEditor.exe RogueRigs.uproject <map> -game -ResX=1920 -ResY=1080 -windowed` with `-csvprofile` or `stat unit` logging, executes the steps (50, 100, 200, 300, 400; about 20 s each after a 5 s settle) and writes average and p99 frame, game, draw and GPU ms per step; results and procedure in [[stress-test-m1]] (Baseline section), new leads in [[perf-risks]]. Commit the script, not the CSV dumps.
- done_when: build exits 0; tests pass headless; the -game run completed on this PC and the table exists with real numbers; lint clean.
- editor_state: closed (the run launches its own -game process)
- token_ceiling: 90000
- milestone: M1
- result: open

## Relations
- [[task-013-stress-test]] — parent global task.
