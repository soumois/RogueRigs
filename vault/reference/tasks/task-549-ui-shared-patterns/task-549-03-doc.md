---
id: task-549-03-doc
description: "Doc: Write the UI shared pieces node with the inventory"
parent: task-549-ui-shared-patterns
kind: task
status: done
---
# UI: generalise the patterns and scripts used again and again: doc

- goal: Write the UI shared pieces node with the inventory
- agent: doc
- depends_on: task-549-02-capture
- inputs: umg-layout (vault/reference/tech/conventions/umg-layout.md), sys-button-hover (vault/reference/tech/systems/ui/sys-button-hover.md)
- expected_output: see done_when
- done_when: A1 holds in a tech node linked from tech-index or umg-layout; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-549
- usage: included in the executor run of task-549
- runs: 1
- milestone: M2
- started: 2026-11-14T04:58:17Z
- waiting_for: none
- result: 149401e4: new node ui-shared-pieces under tech-index (A1: shared pieces and tools, inventory table screen / pattern / where, kept-per-screen list with reasons); tool-capture gains ui-screens.js; sys-button-hover and sys-screen-entrance point to UI/RRUIDebugCommands.cpp. Lint clean (warnings only, none on these nodes).

## Relations
- [[task-549-ui-shared-patterns]] — parent global task.
