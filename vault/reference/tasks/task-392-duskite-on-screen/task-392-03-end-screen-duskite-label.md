---
id: task-392-03-end-screen-duskite-label
description: "Bp: WBP_AlertObjectives: run-won ore label "Duskite secured: "; check the other UI assets for a visible "Ore""
parent: task-392-duskite-on-screen
kind: task
status: todo
---
# On-screen ore word: Duskite: bp

- goal: In WBP_AlertObjectives ShowRunEnd, the label string "Ore secured: " -> "Duskite secured: " (number from the task-391 getter kept); nothing else changes. Then read the visible texts (TextBlocks, button labels, format strings) of every widget under /Game/RogueRigs/UI except WBP_Intermission and list any player-visible "Ore" left (tooltips, comments and variable names do not count); report them, do not change other assets.
- agent: bp
- depends_on: task-391-02-run-won-ore-line
- inputs: agent-bp (vault/reference/agents/agent-bp.md), wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md), wbp-ore-counter (vault/reference/tech/blueprints/ui/wbp-ore-counter.md), task-391-02-run-won-ore-line (vault/reference/tasks/task-391-final-screens-count-level-ore/task-391-02-run-won-ore-line.md) result, global request and gdd_check: vault/reference/tasks/task-392-duskite-on-screen.md
- expected_output: WBP_AlertObjectives saved, compiled with warnings as errors; commit `task-392-03-end-screen-duskite-label: ...`; change summary with the list of UI assets checked and any visible "Ore" found.
- done_when: Graph read back: run-won label "Duskite secured: ", Main Menu button and getter intact; compiled with warnings as errors, no error; saved; check list reported; committed by path.
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
- [[task-392-duskite-on-screen]] — parent global task.
