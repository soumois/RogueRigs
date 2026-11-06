---
id: task-325-doc-agent
description: "Global task: add a Sonnet doc agent that writes a task's vault documentation from an executor brief; cpp and bp return a change summary instead."
parent: tasks-done-index
kind: task
status: done
---
# Doc agent for end-of-task vault documentation

- request: relayed by the main session (owner, in French, 2026-10-27), after the cost analysis of task-324-02-heal-numbers-cpp (3.9M raw tokens, 0.69M units, mostly cache read; the docs turns ran at ~130k context and cost ~21% of the cache reads, ~0.8M of 3.7M; a fresh Sonnet agent would start at 30-40k): "Create a new dedicated agent `doc` (Sonnet, NOT Haiku) that writes the vault documentation at the end of a task. It receives a precise handover brief from the executor: the diff (git diff by path), the decided values, and the list of vault nodes to create or update. It follows vault/_authoring.md (frontmatter, parent + ## Relations on both ends, root.md update where needed), runs `node vault/scripts/lint.js --write`, fixes problems, commits by path, and returns a short recap. The executor must pass it the docs step as its own subtask (the implementing agent, e.g. cpp or bp, returns a change summary instead of writing docs itself). Only the docs-writing role is requested; a reader/recap agent is explicitly NOT requested now." Scope: crew definitions only, no game content, no C++. Owner answer (2026-10-27): "ok", every default Q1 to Q4 below accepted and the plan confirmed, plus this addition: "Cost tracking of the doc agent: it must be run as a real subtask with its own node (description starting with the subtask id task-NNN-SS, so vault/scripts/agent-usage.js attributes it and record-cost.js gives it a cost-log row, usage line, model and alerts). [...] The owner follows costs through a mod reading usage-snapshot.json and cost-log, so it must appear there like cpp and bp." Crew-board icon (owner addition, checked by the Planner): `Tools/crew-board/hooks/register.tsx` falls back to the `claude` icon (`ICON_SVG[name] ?? ICON_SVG.claude`) and a grey `◉` glyph (`AGENTS[name] ?? ...`) for an unknown agent, so `doc` shows the fallback; a dedicated icon needs a new image and is crew-board mod work, outside this task.
- goal: a `doc` agent (Sonnet) exists and is wired in: the Planner plans a docs subtask for it, the Executor delegates it with a brief, cpp and bp stop writing vault docs and return a change summary.
- acceptance: see [[task-325-01-doc-agent]] `done_when`; committed.
- gdd_check: dev tooling, no gdd node needed (Planner, 2026-10-27). Rule nodes edited with explicit owner intent (this request): [[orchestration-protocol]] (steps 3 and 5). Precedent for who edits crew definitions: tasks 320 to 323, one cpp subtask each. Decided (owner "ok" to the defaults, 2026-10-27, provisional): Q1 the docs split applies to cpp and bp only; art, 3d, level-design and text keep writing their own nodes, because those nodes are their output. Q2 one `doc` subtask at the end of each global task that has cpp or bp subtasks, planned by the Planner when it writes the plan, depends_on every implementing subtask; the Executor's brief gives the commit hashes and changed paths (the doc agent runs `git show <hash> -- <paths>` itself, the diff is not pasted into the prompt), the decided values (pointer to the global `request`/`gdd_check`), the implementers' change summaries and the list of nodes to create or update. Q3 the open tasks already planned (308, 309, 314) are not re-planned: their cpp and bp subtasks keep writing docs as written. Q4 doc agent definition: model sonnet, tools Read, Write, Edit, Bash, Glob (no Agent, no Unreal MCP), maxTurns 30, `token_ceiling` set by [[task-326-units-ceiling]] (written `open` here). The Blueprint doc rule of [[_authoring]] still holds: the task is not done until the doc subtask updates the `bp-*` node.
- confirmed_by_owner: yes (2026-10-27)
- milestone: M2
- next_step: none
- started: 2026-10-27T14:17:20Z
- waiting_for: none
- executor_tokens: input 14, cache write 34953, cache read 165396, output 2935
- executor_usage: runs 1, turns 7, context start 16k peak 35k, resumes 0, 3 min, units 0.07 M
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Done. Doc agent created (.claude/agents/doc.md, agent-doc) and wired into executor, planner, cpp, bp, protocol steps 3 and 5, runbook, agents-index; commit c78c7bea. Q3: cpp/bp keep an exception for tasks 308, 309, 314.

Run order: task-325 before task-326 (326-02 is run by the doc agent).

## Subtasks
1. task-325-01-doc-agent — cpp, editor any: doc agent definition files, executor, planner, cpp and bp definitions, protocol, runbook, agents-index.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-325-01-doc-agent]] — step 1, cpp.
- [[task-326-units-ceiling]] — sibling owner point (ceiling measure), run after; sets the doc agent's ceiling.
