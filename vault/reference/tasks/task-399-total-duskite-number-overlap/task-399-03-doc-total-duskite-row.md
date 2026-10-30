---
id: task-399-03-doc-total-duskite-row
description: "Doc: update the wbp-intermission doc: ore band layout after the fix"
parent: task-399-total-duskite-number-overlap
kind: task
status: in-progress
---
# Stats screen: Total Duskite number overlaps its label: doc

- goal: Update wbp-intermission: components (OreRow if added), anchor_rule ore band values after task-399-01, the Q1 decision and its umg-layout exception if A, verified line from the capture report (or "owner checks" if no capture). Nothing else.
- agent: doc
- depends_on: task-399-01-total-duskite-row, task-399-02-total-duskite-capture
- inputs: agent-doc (vault/reference/agents/agent-doc.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md), global request, gdd_check and Questions: vault/reference/tasks/task-399-total-duskite-number-overlap.md
- expected_output: wbp-intermission updated, lint clean, commit `task-399-03-doc-total-duskite-row: ...`.
- done_when: wbp-intermission lists the ore band widgets with anchors, alignment, offsets or slot paddings after the fix, the task-399 reference and the verification state; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-30T23:42:38Z
- waiting_for: none
- result: open

## Relations
- [[task-399-total-duskite-number-overlap]] — parent global task.
