---
id: task-478-03-doc
description: "Doc: Docs describe the changed wall material."
parent: task-478-wall-lines-emissive-edge
kind: task
status: in-progress
---
# Walls without black lines, emissive on the top edge: doc

- goal: Docs describe the changed wall material.
- agent: doc
- depends_on: task-478-02-capture
- inputs: task-471-wall-look-reference (vault/reference/tasks/task-471-wall-look-reference.md), visual-style (vault/reference/gdd/art-audio/visual-style.md)
- expected_output: see done_when
- done_when: The bp-* node of the wall material describes the removed strata, the emissive placement and the side shading; lint clean; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-06T20:17:23Z
- waiting_for: none
- result: open

## Relations
- [[task-478-wall-lines-emissive-edge]] — parent global task.
