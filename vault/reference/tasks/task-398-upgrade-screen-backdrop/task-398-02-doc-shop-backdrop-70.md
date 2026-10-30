---
id: task-398-02-doc-shop-backdrop-70
description: "Doc: update wbp-shop-screen (70 % backdrop, cause) and note the WBP_UpgradeChoice finding in wbp-upgrade-choice"
parent: task-398-upgrade-screen-backdrop
kind: task
status: in-progress
---
# Spare Parts screen: black backdrop at 70 %: doc

- goal: Record in wbp-shop-screen the backdrop state before, the cause, the 70 % value after (and correct its description "opaque black screen"); record in wbp-upgrade-choice the rendered-state finding of its Backdrop.
- agent: doc
- depends_on: task-398-01-shop-backdrop-70
- inputs: agent-doc (vault/reference/agents/agent-doc.md), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md), wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), global request and gdd_check: vault/reference/tasks/task-398-upgrade-screen-backdrop.md
- expected_output: wbp-shop-screen and wbp-upgrade-choice updated; commit `task-398-02-doc-shop-backdrop-70: ...`.
- done_when: wbp-shop-screen states alpha 0.70, the previous state and the cause; wbp-upgrade-choice states the finding; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-30T22:40:31Z
- waiting_for: none
- result: open

## Relations
- [[task-398-upgrade-screen-backdrop]] — parent global task.
