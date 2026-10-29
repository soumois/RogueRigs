---
id: task-362-01-agents-path-of-doc
description: "Doc: style-guide definition and node get fixed read paths; 3d, art, bp, cpp, doc, text definitions and nodes resolve wikilinks with path-of.js."
parent: task-362-agents-path-of
kind: task
status: done
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
- model: sonnet
- tokens: input 14, cache write 33711, cache read 138577, output 4163
- usage: turns 7, context start 11k peak 34k, resumes 0, 3 min, units 0.08 M
- runs: 1
- milestone: M2
- started: 2026-10-29T15:22:12Z
- waiting_for: none
- result: Done, commit 98d28b42: style-guide has its nine read paths, six definitions and nodes use path-of.js. grep -l _paths still lists the seven files because the mandated sentence says 'never open vault/_paths.md'; none tells the agent to read it (intent met).

## Relations
- [[task-362-agents-path-of]] — parent global task.
