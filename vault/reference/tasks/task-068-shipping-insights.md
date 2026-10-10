---
id: task-068-shipping-insights
description: "Global task: Measurement in a packaged build with an Unreal Insights capture of the isolated frame spikes."
parent: tasks-index
kind: task
status: done
---
# Packaged build and Insights

- request: "Une mesure dans un build packagé avec Unreal Insights" (proposal after task-066-02, to run at night) (owner, 2026-10-09)
- goal: Measurement in a packaged build with an Unreal Insights capture of the isolated frame spikes.
- acceptance: spikes explained or listed with their trace evidence; results written in stress-grid-cells; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-09).
- confirmed_by_owner: yes (2026-10-09, planned; the owner launches it at night)
- milestone: M2
- next_step: none
- result: Done. Development packaged build (Test not available on the launcher engine). Packaged saves 2.0-3.5 ms of game thread; game 250 p99 22.3 ms packaged vs 30.1 editor. S0 21 ms spike: D3D12 submit stall (GPU/driver), dig 57.9 ms not reproduced; untagged real-game spikes: XP bursts (mass deaths drop cubes in one frame, 0.37 ms per robot) and wave spawns (0.74 ms per robot); packaged GC never spikes. Top cost: CharacterMovement 4.9 ms of 12.5 ms at 250 robots. Details in perf-trace-real-game.

## Subtasks
1. task-068-01-shipping-insights — cpp

## Relations
- [[tasks-index]] — parent index.
- [[task-068-01-shipping-insights]] — step 1, cpp.
