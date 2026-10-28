---
id: task-357-03-shop-indicator-doc
description: "Doc: update wbp-shop-screen with the IndicatorRessource tree and anchors, from task-357-01 and its capture."
parent: task-357-shop-resource-indicator
kind: task
status: todo
---
# Shop screen: resource indicator centred on its font: doc

- goal: update wbp-shop-screen from the commit and change summary of task-357-01 and the report of 357-02: IndicatorRessource tree (no Row), anchors and alignment of CounterBg, OreIcon and OreBalanceText, how the icon size follows the font, verified line with resolutions, digit counts and what is unseen.
- agent: doc
- depends_on: task-357-01-shop-resource-indicator, task-357-02-shop-indicator-capture
- inputs: wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md), global task-357-shop-resource-indicator `request` and `## Questions`
- expected_output: the node updated (split per _authoring if over ~400 words); one commit.
- done_when: the node matches the commit; anchor values present for the three indicator widgets; lint clean; committed by path.
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
- [[task-357-shop-resource-indicator]] — parent global task.
