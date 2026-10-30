---
id: task-391-02-run-won-ore-line
description: "Bp: WBP_AlertObjectives: run-won ore line reads the run total getter"
parent: task-391-final-screens-count-level-ore
kind: task
status: todo
---
# End screens count the ore of the level just extracted: bp

- goal: In WBP_AlertObjectives ShowRunEnd, on the run-won branch, EndOreText = "Ore secured: " + the task-391-01 run total getter instead of GetSecuredOre(RunFlow); other branches unchanged. Starts from the widget as left by task-390-02.
- agent: bp
- depends_on: task-391-01-level-ore-in-recap, task-390-02-end-screen-menu-button
- inputs: agent-bp (vault/reference/agents/agent-bp.md), wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), task-391-01-level-ore-in-recap (vault/reference/tasks/task-391-final-screens-count-level-ore/task-391-01-level-ore-in-recap.md) change summary (getter name), task-390-02-end-screen-menu-button (vault/reference/tasks/task-390-run-won-back-to-main-menu/task-390-02-end-screen-menu-button.md) result, global request and gdd_check: vault/reference/tasks/task-391-final-screens-count-level-ore.md
- expected_output: WBP_AlertObjectives saved, compiled with warnings as errors; commit `task-391-02-run-won-ore-line: ...`; change summary for the doc subtask.
- done_when: Graph read back: run-won branch uses the getter, other branches unchanged, Main Menu button of task-390 intact; compiled with warnings as errors, no error; saved; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-391-final-screens-count-level-ore]] — parent global task.
