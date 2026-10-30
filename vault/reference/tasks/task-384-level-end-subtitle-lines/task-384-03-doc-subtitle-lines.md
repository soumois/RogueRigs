---
id: task-384-03-doc-subtitle-lines
description: "Doc: update the wbp-intermission doc node with the subtitle line placement and the capture result."
parent: task-384-level-end-subtitle-lines
kind: task
status: todo
---
# Stats screen subtitle lines: doc

- goal: Update wbp-intermission from the change summary of task-384-01 (git show of its commit) and the capture report of task-384-02: anchor_rule of SubtitleLineL/R (placed by the graph, offsets computed), events (placement step), verified (defect closed or not, unseen items). Keep the node near 400 words.
- agent: doc
- depends_on: task-384-01-subtitle-lines-widget, task-384-02-subtitle-lines-capture
- inputs: wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md), agent-doc (vault/reference/agents/agent-doc.md), Global request, gdd_check and Questions of task-384-level-end-subtitle-lines for the decided values
- expected_output: wbp-intermission updated, lint clean, one commit `task-384-03-doc-subtitle-lines: <summary>`.
- done_when: wbp-intermission describes the line placement and the capture result, the level 4 crossing defect no longer listed as open if C1 is seen; lint clean; committed by path.
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
- [[task-384-level-end-subtitle-lines]] — parent global task.
