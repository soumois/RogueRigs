---
id: task-397-02-doc-extractor-out-of-sight
description: "Doc: update the shuttle, run flow and preload docs with the out-of-sight rule"
parent: task-397-shuttle-stays-gone
kind: task
status: todo
---
# Extractor stays gone after its takeoff: doc

- goal: Record the cause, the fix and the log verification.
- agent: doc
- depends_on: task-397-01-extractor-out-of-sight
- inputs: agent-doc (vault/reference/agents/agent-doc.md), sys-objectives-shuttle (vault/reference/tech/systems/world/sys-objectives/sys-objectives-shuttle.md), sys-objectives-run-flow (vault/reference/tech/systems/world/sys-objectives/sys-objectives-run-flow.md), sys-run-transition-preload (vault/reference/tech/systems/world/sys-run/sys-run-transition-preload.md), global request and gdd_check: vault/reference/tasks/task-397-shuttle-stays-gone.md
- expected_output: The listed doc nodes updated; commit `task-397-02-doc-extractor-out-of-sight: ...`.
- done_when: Cause, fix and log lines stated; lint clean; committed by path.
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
- [[task-397-shuttle-stays-gone]] — parent global task.
