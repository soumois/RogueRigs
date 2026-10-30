---
id: task-391-03-doc-level-ore
description: "Doc: update the run, transition, ore counter and end screen docs with the corrected ore count"
parent: task-391-final-screens-count-level-ore
kind: task
status: cancelled
---
# Run-won end screen shows the run ore total: doc

- goal: Record the cause, the fix, the getter, the end screen change and the log verification.
- agent: doc
- depends_on: task-391-01-level-ore-in-recap, task-391-02-run-won-ore-line
- inputs: agent-doc (vault/reference/agents/agent-doc.md), sys-run (vault/reference/tech/systems/world/sys-run.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-objectives-ore-counter (vault/reference/tech/systems/world/sys-objectives/sys-objectives-ore-counter.md), wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), global request and gdd_check: vault/reference/tasks/task-391-final-screens-count-level-ore.md
- expected_output: The listed doc nodes updated; commit `task-391-03-doc-level-ore: ...`.
- done_when: Cause, fix, getter and end screen branch stated; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: Cancelled 2026-10-30, replaced by task-391-05-doc-ore-balance after the owner spec change (balance everywhere).

## Relations
- [[task-391-final-screens-count-level-ore]] — parent global task.
