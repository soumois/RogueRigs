---
id: task-392-05-doc-duskite-labels
description: "Doc: update the end screen and stats screen widget docs with the Duskite labels"
parent: task-392-duskite-on-screen
kind: task
status: todo
---
# On-screen ore word: Duskite: doc

- goal: Record the new label strings and the check list of UI assets.
- agent: doc
- depends_on: task-392-03-end-screen-duskite-label, task-392-04-intermission-duskite-label
- inputs: agent-doc (vault/reference/agents/agent-doc.md), wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md), global request and gdd_check: vault/reference/tasks/task-392-duskite-on-screen.md
- expected_output: wbp-alert-objectives and wbp-intermission updated; commit `task-392-05-doc-duskite-labels: ...`.
- done_when: Both nodes state the Duskite labels and the check list; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
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
