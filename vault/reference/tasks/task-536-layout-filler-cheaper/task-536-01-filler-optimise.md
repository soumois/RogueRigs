---
id: task-536-01-filler-optimise
description: "Cpp: Time a slow seed, make RunFiller cheaper without changing its output, prove identical output on the seed set and time it again."
parent: task-536-layout-filler-cheaper
kind: task
status: in-progress
---
# Layout generator: cheaper far-cell filler, identical output: cpp

- goal: Time a slow seed, make RunFiller cheaper without changing its output, prove identical output on the seed set and time it again.
- agent: cpp
- depends_on: none
- inputs: sys-layout-generator (vault/reference/tech/systems/world/sys-grid/sys-layout-generator.md), Source/RogueRigs/Grid/RRLayoutGenerator.cpp, Source/RogueRigs/Grid/RRLayoutGenerator.h
- expected_output: C++ change committed; before/after timing table and identity comparison (seed, hash before, hash after) in the subtask result.
- done_when: A1 to A6 of the global hold: before and after times of the slow seed with the filler share recorded in the subtask result, reference seed 422704499 / hash 86c36f6d reproduced, -carveonly hash and JSON byte-identical before and after on the seed set of Q2, every tried seed of the reference search logs the same line, build and existing generator tests pass, no output-changing track implemented.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-13T18:19:35Z
- waiting_for: none
- result: open

## Relations
- [[task-536-layout-filler-cheaper]] — parent global task.
