---
id: task-383-04-doc-total-ore
description: "Doc: update the wbp-intermission doc node with the Total Ore line, the moved anchors and the capture result."
parent: task-383-level-end-total-ore
kind: task
status: todo
---
# Stats screen Total Ore: doc

- goal: Update wbp-intermission from the change summaries of task-383-01 and task-383-02 (git show of their commits) and the capture report of task-383-03: purpose (one Total Ore line), components (ore texts), anchor_rule (every moved widget), events (Lines[0], RunSecuredOre), verified (capture result, unseen items; the 1920 x 1200 and 2560 x 1080 frames now after the button fix). Keep the node near 400 words: shorten history lines rather than add.
- agent: doc
- depends_on: task-383-01-total-ore-text, task-383-02-total-ore-widget, task-383-03-total-ore-capture
- inputs: wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md), agent-doc (vault/reference/agents/agent-doc.md), Global request, gdd_check and Questions of task-383-level-end-total-ore for the decided values
- expected_output: wbp-intermission updated, lint clean, one commit `task-383-04-doc-total-ore: <summary>`.
- done_when: wbp-intermission no longer mentions OreLevelText, OreRunText or Lines[0..1] as current; it lists the Total Ore texts with their anchors, the new anchors of the moved widgets, the graph change and the capture result; lint clean; committed by path.
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
- [[task-383-level-end-total-ore]] — parent global task.
