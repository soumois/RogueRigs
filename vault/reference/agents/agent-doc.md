---
id: agent-doc
description: "Doc agent: writes a global task's sys-* and bp-* nodes from the Executor's brief, in its own subtask. Open to run or change it."
parent: agents-index
kind: spec
status: provisional
---
# Doc agent

- role: write the vault documentation at the end of a global task that has cpp or bp subtasks, in a fresh small context instead of the implementer's large one (owner 2026-10-27, [[task-325-doc-agent]]).
- invoked_by: [[agent-executor]], through the doc subtask the [[agent-planner]] plans last, depending on every implementing subtask.
- trigger: a subtask node with `agent: doc`.
- reads: its subtask node; [[_authoring]]; the templates `vault/templates/system.md` and `vault/templates/blueprint.md` when it writes such a node; the nodes and files the brief lists. Nothing else.
- wikilinks: resolved with `node vault/scripts/path-of.js <id> [<id>...]` (exact ids), never by opening `vault/_paths.md`.
- brief: written by the Executor in the delegation prompt: commit hashes and changed paths (it runs `git show <hash> -- <paths>` itself, the diff is not pasted), decided values (pointer to the global `request` and `gdd_check`), the implementers' change summaries, the list of nodes to create or update.
- produces: the listed `sys-*`, `bp-*` and tool doc nodes created or updated, with `parent` and `## Relations` on both ends, `root.md` only when a branch changes; one commit.
- output_format: nodes from `vault/templates/system.md` or `vault/templates/blueprint.md` per [[_authoring]]; report of a few lines: nodes created, nodes updated, values left `open`, lint result, commit hash.
- definition_of_done: every listed node matches the commits; lint clean; committed by explicit path.
- must_not: write or edit code, Blueprints, assets, maps or Config; change a gdd value; edit task status or cost fields; search the vault; document anything the brief does not list; invent a value (write `open` and report it).
- scope: cpp and bp only; art, 3d, level-design and text keep writing their own nodes (Q1 of [[task-325-doc-agent]]).
- definition_file: none; removed 2026-11-11 (owner, task-503 Q9): the Executor applies these rules itself.
- model: sonnet
- token_ceiling: 0.3 M units (provisional, owner 2026-10-27)

## Relations
- [[agents-index]] — parent index.
- [[agent-executor]] — delegates the doc subtask and writes the brief.
- [[agent-planner]] — plans the doc subtask.
- [[agent-cpp]] — returns the change summary it documents.
- [[agent-bp]] — returns the change summary it documents.
- [[_authoring]] — the writing contract it follows.
