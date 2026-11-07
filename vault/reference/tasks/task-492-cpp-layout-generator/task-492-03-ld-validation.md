---
id: task-492-03-ld-validation
description: "Level-design: Level-Design agent generates 20 passing seeds with the C++ generator and reviews them with layout-review-rules."
parent: task-492-cpp-layout-generator
kind: task
status: in-progress
---
# C++ layout generator writing the grid JSON, validated by the Level-Design agent: level-design

- goal: Level-Design agent generates 20 passing seeds with the C++ generator and reviews them with layout-review-rules.
- agent: level-design
- depends_on: task-492-02-gen-place-json
- inputs: agent-level-design (vault/reference/agents/agent-level-design.md), layout-review-rules (vault/reference/tech/pipelines/layout-review-rules.md), layout-generator-recipe (vault/reference/tech/pipelines/layout-generator-recipe.md), level-layout-schema (vault/reference/tech/pipelines/level-layout-schema.md)
- expected_output: Generated files and the review table go to Saved/GeneratedLevels/task-492/ (temporary, not committed); the verdict (per-seed window min and max, flags, kept seeds, comparison with the Node generator) is written in this subtask node.
- done_when: A4 of the global holds: 20 seeds validated by validate_layout.js, verdict per seed written, defects found fixed in the generator and re-checked, remaining deviations listed for the owner; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-07T06:27:26Z
- waiting_for: none
- result: open

## Relations
- [[task-492-cpp-layout-generator]] — parent global task.
