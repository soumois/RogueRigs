---
id: task-362-01-agents-path-of-doc
description: "Doc: style-guide definition and node get fixed read paths; 3d, art, bp, cpp, doc, text definitions and nodes resolve wikilinks with path-of.js."
parent: task-362-agents-path-of
kind: task
status: todo
---
# Agent definitions: path-of.js instead of _paths.md: doc

- goal: Apply the decided Q1 and Q2 of the global (read its `request`, `gdd_check` and `## Questions`): edit `.claude/agents/style-guide.md` step 1 and agent-style-guide; replace the `_paths.md` sentence in step 1 of the six definitions and add one clause to their six vault nodes. Change nothing else in those files.
- agent: doc
- depends_on: none
- inputs: task-362-agents-path-of (vault/reference/tasks/task-362-agents-path-of.md), `.claude/agents/style-guide.md`, agent-style-guide (vault/reference/agents/agent-style-guide.md), `.claude/agents/3d.md`, `.claude/agents/art.md`, `.claude/agents/bp.md`, `.claude/agents/cpp.md`, `.claude/agents/doc.md`, `.claude/agents/text.md`, agent-3d (vault/reference/agents/agent-3d.md), agent-art (vault/reference/agents/agent-art.md), agent-bp (vault/reference/agents/agent-bp.md), agent-cpp (vault/reference/agents/agent-cpp.md), agent-doc (vault/reference/agents/agent-doc.md), agent-text (vault/reference/agents/agent-text.md), _authoring (vault/_authoring.md)
- expected_output: the listed definition files and agent nodes updated, one commit by path.
- done_when: style-guide.md step 1 lists the nine resolved paths of the global gdd_check, says the entity path comes from the brief, never to open `vault/_paths.md` and to stop and report a missing path, tools still `Read, Write`; agent-style-guide says the same in one clause; `grep -l _paths .claude/agents/*.md` prints only planner.md; each of the six definitions names `node vault/scripts/path-of.js` in step 1 and each of the six nodes states it; every path listed in style-guide.md exists; `node vault/scripts/lint.js --write` clean; committed by path.
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
- [[task-362-agents-path-of]] — parent global task.
