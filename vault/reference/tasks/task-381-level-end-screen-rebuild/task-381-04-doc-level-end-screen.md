---
id: task-381-04-doc-level-end-screen
description: "Doc: update the wbp-intermission doc node with the rebuilt tree, anchors, brushes and verification."
parent: task-381-level-end-screen-rebuild
kind: task
status: todo
---
# Level-end screen rebuild: doc

- goal: Update the doc node from the change summaries of task-381-01 and task-381-02 (git show of their commits) and the capture report of task-381-03: components (new tree), an anchor_rule list per top-level widget (anchors min/max, alignment, offsets, size mode, as wbp-shop-screen) so the owner can edit by hand, assets (the three LevelEnd textures and the reused T_Shop_TitleBar, T_Shop_ButtonContinue, T_Icon_Ore with their Box margins), events (graph changes for the two-part lists and upper case), verified (capture result, unseen items).
- agent: doc
- depends_on: task-381-01-level-end-sprites-import, task-381-02-intermission-widget-rebuild, task-381-03-level-end-screen-capture
- inputs: wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md), agent-doc (vault/reference/agents/agent-doc.md), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md) (anchor_rule form to follow), Global request, gdd_check and Questions of task-381-level-end-screen-rebuild for the decided values
- expected_output: wbp-intermission updated, under about 400 words, lint clean, one commit `task-381-04-doc-level-end-screen: <summary>`.
- done_when: wbp-intermission lists the new tree, the anchors per top-level widget, the textures and Box margins, the graph changes and the capture result with its unseen items; lint clean; committed by path.
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
- [[task-381-level-end-screen-rebuild]] — parent global task.
