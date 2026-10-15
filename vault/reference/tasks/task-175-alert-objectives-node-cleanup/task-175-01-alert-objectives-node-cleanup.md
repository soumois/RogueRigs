---
id: task-175-01-alert-objectives-node-cleanup
description: "Delete the about 45 empty, unconnected custom event nodes left in the WBP_AlertObjectives event graph by failed graph writes of task-168-07."
parent: task-175-alert-objectives-node-cleanup
kind: task
status: done
---
# WBP_AlertObjectives: unused node cleanup

- goal: Delete the about 45 empty, unconnected custom event nodes left in the WBP_AlertObjectives event graph by failed graph writes of task-168-07.
- agent: bp
- depends_on: none
- inputs: wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: WBP_AlertObjectives without the unconnected custom event nodes (names ending _0, _1, _2, CustomEvent, CustomEvent_0 with no wire); the live handlers OnUpgradeOfferShown_Event_2 / OnUpgradeOfferClosed_Event_2 and every wired node kept; wbp-alert-objectives doc no longer lists the leftover nodes.
- done_when: Every deleted node had no connected pin (listed in the report); compiles with warnings as errors; saved; graph read back; doc updated.
- editor_state: open
- token_ceiling: 40 turns
- model: sonnet
- tokens: input 22, cache write 42664, cache read 277398, output 429
- runs: 1
- milestone: M2
- result: 33 unconnected K2Node_CustomEvent nodes deleted from the EventGraph (16 graphs, 329 nodes scanned; no other graph had any); every wired node and the live offer handlers kept; the unconnected EventPreConstruct stub left in place. Compiled with warnings as errors, saved, read back, doc updated. Commit 25ebe87.

## Relations
- [[task-175-alert-objectives-node-cleanup]] — parent global task.
