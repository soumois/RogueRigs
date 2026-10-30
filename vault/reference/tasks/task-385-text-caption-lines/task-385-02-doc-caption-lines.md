---
id: task-385-02-doc-caption-lines
description: "Doc: update the text-content-files lines row with the caption exception."
parent: task-385-text-caption-lines
kind: task
status: todo
---
# Text validator caption lines: doc

- goal: Update the `lines` row of the screen_texts.json table in text-content-files from the change summary of task-385-01 (git show of its commit): "each 3 to 12 words; an entry ending with ':' is a caption for a value drawn beside it: 1 to 4 words (owner 2026-10-30, task-385)". Nothing else in the node changes.
- agent: doc
- depends_on: task-385-01-validator-caption-lines
- inputs: text-content-files (vault/reference/tech/pipelines/text-content-files.md), agent-doc (vault/reference/agents/agent-doc.md), Global request and Questions of task-385-text-caption-lines for the decided rule
- expected_output: text-content-files updated, lint clean, one commit `task-385-02-doc-caption-lines: <summary>`.
- done_when: the lines row states the exception with its owner date; lint clean; committed by path.
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
- [[task-385-text-caption-lines]] — parent global task.
