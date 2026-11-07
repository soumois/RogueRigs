---
id: task-495-02-ld-review
description: "Level-design: dev-time re-review of the 24 seed starts of task-492-03 against all layout-review-rules, compared with review_cpp.md."
parent: task-495-generator-window-repair
kind: task
status: in-progress
---
# C++ generator enforces the layout review rules: level-design

- goal: Dev-time level-design re-review of the 24 seed starts of task-492-03 against all layout-review-rules, compared with review_cpp.md; an independent check of the generator, not part of the game.
- agent: level-design
- depends_on: task-495-01-repair-pass
- inputs: layout-review-rules (vault/reference/tech/pipelines/layout-review-rules.md), task-492-03-ld-validation (vault/reference/tasks/task-492-cpp-layout-generator/task-492-03-ld-validation.md), Saved/GeneratedLevels/task-492/review_cpp.md
- expected_output: see done_when
- done_when: A new table next to review_cpp.md gives per start: accepted seed, validator result, min and max window, straight lane flag, pocket distance, verdict; 24/24 meet the window rule, the straight-lane rule (30 cells, ±1 tolerance) and the pocket distance rule (15 cells), measured independently of the generator report, and pass the validator; A4 judged on the downscaled previews with any unnatural repaired area named.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-07T07:52:59Z
- waiting_for: none
- result: open

## Relations
- [[task-495-generator-window-repair]] — parent global task.
