---
id: task-398-02-doc-backdrop-25
description: "Doc: update the chosen widget doc with the 25 % backdrop"
parent: task-398-upgrade-screen-backdrop
kind: task
status: todo
---
# Upgrade screen: black backdrop at 25 %: doc

- goal: Record the backdrop value before and after in the doc of the changed widget.
- agent: doc
- depends_on: task-398-01-backdrop-25
- inputs: agent-doc (vault/reference/agents/agent-doc.md), wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md), global request and Questions: vault/reference/tasks/task-398-upgrade-screen-backdrop.md
- expected_output: The doc node of the changed widget updated; commit `task-398-02-doc-backdrop-25: ...`.
- done_when: The node states alpha 0.25 and the previous value; lint clean; committed by path.
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
- [[task-398-upgrade-screen-backdrop]] — parent global task.
