---
id: task-545-04-doc
description: "Doc: Update the pause widget doc and document the shared bolt and bar pieces"
parent: task-545-pause-screen-bolts
kind: task
status: done
---
# Pause screen: grid and bolts on the frame bars as owner image 10: doc

- goal: Update the pause widget doc and document the shared bolt and bar pieces
- agent: doc
- depends_on: task-545-03-capture
- inputs: pause-menu (vault/reference/gdd/run/pause-menu.md), umg-layout (vault/reference/tech/conventions/umg-layout.md)
- expected_output: see done_when
- done_when: Pause bp node updated; the shared pieces and their use described; art listed; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-545
- usage: included in the executor run of task-545
- runs: 1
- milestone: M2
- started: 2026-11-14T03:50:00Z
- waiting_for: none
- result: Commit 1f7c96e4. New node wbp-ui-bolted-bar (both shared widgets, layout table, how to place, Art/Source/ui-frame-pieces assets listed as keep, style-guide state); wbp-pause-menu anchors table, grid_and_bolts entry and relation updated; blueprints-ui-index child added; lint clean.

## Relations
- [[task-545-pause-screen-bolts]] — parent global task.
