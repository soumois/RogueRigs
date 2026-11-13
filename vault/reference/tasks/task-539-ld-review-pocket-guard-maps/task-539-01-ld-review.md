---
id: task-539-01-ld-review
description: "Level-design: Before/after visual and measured review of the 7 starts whose accepted seed changed after task-537."
parent: task-539-ld-review-pocket-guard-maps
kind: task
status: in-progress
---
# Revue visuelle Level-Design des cartes changées par la libération du garde de poche: level-design

- goal: Before/after visual and measured review of the 7 starts whose accepted seed changed after task-537.
- agent: level-design
- depends_on: none
- inputs: layout-review-rules (vault/reference/tech/pipelines/layout-review-rules.md), task-495-02-ld-review (vault/reference/tasks/task-495-generator-window-repair/task-495-02-ld-review.md), task-492-03-ld-validation (vault/reference/tasks/task-492-cpp-layout-generator/task-492-03-ld-validation.md), task-537-layout-stale-pocket-guard (vault/reference/tasks/task-537-layout-stale-pocket-guard.md), sys-layout-generator (vault/reference/tech/systems/world/sys-grid/sys-layout-generator.md), Saved/GeneratedLevels/task-537/, Tools/layout_gen/params/fine_default.json
- expected_output: Table, previews and the sheet in a Saved/GeneratedLevels/ folder named after this task (temporary, not committed); verdict in this subtask node; one line in layout-review-rules Automation.
- done_when: A1 to A4 of the global hold; the sheet path and the per-start verdict are in result; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-13T20:18:51Z
- waiting_for: none
- result: open

## Relations
- [[task-539-ld-review-pocket-guard-maps]] — parent global task.
