---
id: task-495-01-repair-pass
description: "Cpp: repair passes and seed-acceptance checks in FRRLayoutGenerator for windows, straight lanes and pocket distance, keeping validator constraints and determinism."
parent: task-495-generator-window-repair
kind: task
status: in-progress
---
# C++ generator enforces the layout review rules: cpp

- goal: Repair passes and seed-acceptance checks in FRRLayoutGenerator for the three review rules, keeping every recipe and validator constraint and determinism. Windows: carve into dead windows, add rock into too-open ones. Straight lanes over 30 cells: break them (for example a wall outcrop) where feasible. Pocket distance over 15: acceptance check (repair if feasible). A layout still breaking a rule after repair fails the seed and the search moves on (Q2).
- agent: cpp
- depends_on: none
- inputs: sys-layout-generator (vault/reference/tech/systems/world/sys-grid/sys-layout-generator.md), layout-generator-recipe (vault/reference/tech/pipelines/layout-generator-recipe.md), layout-review-rules (vault/reference/tech/pipelines/layout-review-rules.md), level-layout-schema (vault/reference/tech/pipelines/level-layout-schema.md), Saved/GeneratedLevels/task-492/review_cpp.md
- expected_output: see done_when
- done_when: A1, A1b, A2, A3, A5 and A6 met on the 24 starts of review_cpp.md, measured by the Executor (window min and max, longest straight lane and farthest pocket per layout, validator output, two-run hash, retries and times) and written in the subtask result; carved floor joins the main network (no new isolated region) and added rock keeps passage 3; the subtask result says which rules are repaired and which only checked.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-07T07:13:43Z
- waiting_for: none
- result: open

## Relations
- [[task-495-generator-window-repair]] — parent global task.
