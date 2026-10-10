---
id: task-068-01-shipping-insights
description: "Measurement in a packaged build with an Unreal Insights capture of the isolated frame spikes."
parent: task-068-shipping-insights
kind: task
status: todo
---
# Packaged build and Insights

- goal: Measurement in a packaged build with an Unreal Insights capture of the isolated frame spikes.
- agent: cpp
- depends_on: task-067-01-real-game-stress
- inputs: build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-stress (vault/reference/tech/systems/sys-stress.md), stress-grid-cells (vault/reference/tech/performance/stress-grid-cells.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: package a Test (or Development if Test cannot run the stress commands) Windows build with BuildCookRun; run the S0 and dig scenarios at 400 robots and at the highest step task-067-01 reached (real-game mode at that MaxAlive) with an Unreal Insights trace (cpu, frame, gpu channels); identify what causes the isolated spikes seen in task-066-02 (one 57.9 ms frame in dig 400, one 21 ms frame in S0 100): GC, streaming, shader compile, or other; compare packaged and editor -game frame times; results in stress-grid-cells; no gameplay change, do not commit build output or traces.
- done_when: spikes explained or listed with their trace evidence; results written in stress-grid-cells; lint clean.
- editor_state: closed
- token_ceiling: 70000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-068-shipping-insights]] — parent global task.
