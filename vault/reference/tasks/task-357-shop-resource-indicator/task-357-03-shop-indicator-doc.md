---
id: task-357-03-shop-indicator-doc
description: "Doc: update wbp-shop-screen with the IndicatorRessource tree and anchors, from task-357-01 and its capture."
parent: task-357-shop-resource-indicator
kind: task
status: done
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
- model: sonnet
- tokens: input 16, cache write 28135, cache read 143907, output 5135
- usage: turns 8, context start 11k peak 28k, resumes 0, 1 min, units 0.08 M
- runs: 1
- milestone: M2
- started: 2026-10-28T21:47:45Z
- waiting_for: none
- result: wbp-shop-screen updated and new node wbp-shop-screen-indicator (08749242); no capture-tool node reachable, --force-res noted in wbp-shop-screen verified_anchors.

## Relations
- [[task-357-shop-resource-indicator]] — parent global task.
