---
id: crew-board-data
description: "Crew board data rules: task files read, status mapping, live agent detection from transcripts, cost attribution to executor and planner. Open before changing a number."
parent: external-tools-index
kind: doc
---
# Crew board: data and rules

- responsibility: say where each number comes from and how it is derived, so a wrong figure can be traced.
- main_files: `Tools/crew-board/hooks/register.tsx` (scan, `pollLive`, `loadUsage`), `usage_of.js`.
- entry_points: a scan every `refreshSeconds` (tasks, costs) and a live poll every 2 s.
- data: `vault/reference/tasks/` (global node plus subtask folder), `vault/scripts/usage-snapshot.json`, `vault/scripts/cost-rates.json` weights, subagent transcripts under `~/.claude/projects/<project>/<session>/subagents/`.
- depends_on: [[orchestrator-runbook]] (the scripts that write statuses and the snapshot), [[cost-log]].
- implements: none.
- gotchas: a transcript older than 3 minutes is never "running"; the module reads 4 MiB at most, hence `usage_of.js`.

## Tasks
Everything is read at start; later only unfinished tasks are re-read (a finished task, done with every subtask done, only when its own node changes), subtasks by modification time. Fields come from the frontmatter and `- key:` lines.

## Statuses shown
To do, In progress, Wait answers, Done, Cancel. Vault `blocked` reads To do (red tag), `review` In progress, `cancelled` Cancel, `wait-answers` Wait answers. Wait answers is also derived: the plan awaits owner confirmation (`confirmed_by_owner: no…`), or the vault says in progress while no live agent is on it.

## Live agents
Running = `finished` false in `agent-<id>.meta.json` and the `.jsonl` written in the last 3 minutes (working under 30 s, else waiting). A worker is linked to its subtask by the node name in its prompt, an executor to its global task; a planner to the tasks it created (`created task-NNN` lines of `task-create.js`, or node files it wrote); other agents are not linked.

## Costs
Subtask: its `tokens` line. Executor and planner: the usage snapshot, plus planners and executors finished since the last snapshot read from their transcripts. A planner that created N tasks is split equally. Units use the weights of `cost-rates.json`. `usage_of.js` counts each API message once, as `agent-usage.js` does.

## Relations
- [[external-tools-index]] — parent index.
- [[crew-board]] — install and screens.
- [[orchestration-protocol]] — who writes the task statuses.
- [[agent-executor]] — the agent whose cost is the executor line.
- [[agent-planner]] — the agent whose cost is the planner line.
