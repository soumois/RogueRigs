---
id: task-359-02-capture-tooling-doc
description: "Doc: new tool doc node tool-capture for capture.js, sheet.js and commands.js under pipelines-index; agent-cpp capture_script points to it."
parent: task-359-capture-tooling
kind: task
status: in-progress
---
# Capture tooling: doc

- goal: Create `vault/reference/tech/pipelines/tool-capture.md` (kind doc: usage of capture.js, sheet.js, commands.js, ffmpeg finder, outputs), add it to pipelines-index Children, and make agent-cpp `capture_script` reference it instead of restating the usage.
- agent: doc
- depends_on: task-359-01-capture-tooling-scripts
- inputs: pipelines-index (vault/reference/tech/pipelines/pipelines-index.md), agent-cpp (vault/reference/agents/agent-cpp.md), `.claude/agents/cpp.md`, _authoring (vault/_authoring.md), `vault/templates/system.md` (doc node shape)
- expected_output: tool-capture node created, pipelines-index and agent-cpp updated, one commit.
- done_when: tool-capture lists the usage line of the three scripts from the change summary; pipelines-index and agent-cpp link it; lint clean; committed.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-29T14:21:50Z
- waiting_for: none
- result: open

## Relations
- [[task-359-capture-tooling]] — parent global task.
