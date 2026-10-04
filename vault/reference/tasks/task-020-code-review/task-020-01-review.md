---
id: task-020-01-review
description: "Read all of Source/RogueRigs: readability, comments, naming, duplication, base-class generalization; write a ranked refactor list with risk."
parent: task-020-code-review
kind: task
status: in-progress
---
# Code review

- goal: know what to refactor and why.
- agent: cpp
- depends_on: task-019-01-asset-manager
- inputs: `Source/RogueRigs/` (all modules), [[cpp-standard]], [[cpp-bp-split]], [[naming]], [[systems-index]].
- expected_output: new node `code-review-m1` under tech conventions (or a new review subfolder with an index if over 400 words): per system, what a new human reader would stumble on (long functions, unclear names, missing or stale comments, magic numbers, test-only hooks in gameplay code, duplicated logic, classes that should share a base: robots, weapons, pickups, objectives, cheats), and a ranked refactor list (gain, risk, hours, Blueprint impact: reflected names that would need CoreRedirects). No code change in this subtask. Also list dead code and files over ~800 lines.
- done_when: the review node exists with concrete file and function references; lint clean.
- editor_state: closed
- token_ceiling: 90000
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-020-code-review]] — parent global task.
