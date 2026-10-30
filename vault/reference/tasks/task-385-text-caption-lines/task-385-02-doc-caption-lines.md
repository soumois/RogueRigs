---
id: task-385-02-doc-caption-lines
description: "Doc: update the text-content-files lines row with the caption exception."
parent: task-385-text-caption-lines
kind: task
status: done
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
- model: sonnet
- tokens: input 8, cache write 18247, cache read 42400, output 1140
- usage: turns 4, context start 11k peak 18k, resumes 0, 0 min, units 0.03 M
- runs: 1
- milestone: M2
- started: 2026-10-30T05:18:55Z
- waiting_for: none
- result: text-content-files lines row states the caption exception (owner 2026-10-30, task-385); commit 10921d62

## Relations
- [[task-385-text-caption-lines]] — parent global task.
