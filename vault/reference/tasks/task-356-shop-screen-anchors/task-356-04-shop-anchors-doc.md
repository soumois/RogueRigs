---
id: task-356-04-shop-anchors-doc
description: "Doc: update wbp-shop-screen and wbp-shop-card with the anchored layout, stating which widget is anchored where with which values."
parent: task-356-shop-screen-anchors
kind: task
status: todo
---
# Shop screen: Canvas and anchors: doc

- goal: update the two Blueprint doc nodes from the commits and change summaries of task-356-01 and 02 and the capture report of 03: replace the fixed-pixel component descriptions by the anchored tree; add a short "anchor rule" field per node (each Root child: anchors min/max, alignment, offsets, size mode; how the card follows its slot) readable by the owner who will edit by hand; record the owner's manual-edits commit; verified line with the four resolutions and what is unseen. Keep the history lines short (point to the tasks).
- agent: doc
- depends_on: task-356-01-shop-screen-anchors, task-356-02-shop-card-fit, task-356-03-shop-anchors-capture
- inputs: wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md), wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md), global task-356-shop-screen-anchors `request` and `## Questions`
- expected_output: the two nodes updated (under ~400 words each; split per _authoring if needed); one commit.
- done_when: both nodes match the commits; anchor values present for every Root child of WBP_ShopScreen and for the card slot; lint clean; committed by path.
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
- [[task-356-shop-screen-anchors]] — parent global task.
