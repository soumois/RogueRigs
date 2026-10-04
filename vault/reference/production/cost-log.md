---
id: cost-log
description: "Tokens per subtask, agent model and runs for tasks 013 onward, with totals per task and model; read to estimate a future project's cost."
parent: production-index
kind: note
---
# Cost log

- Source: the final `subagent_tokens` the Agent tool reports for a subtask (all its runs; a resumed agent reports a running total). It is a size indicator, not the bill: each agent turn re-reads its context, so billed input is several times higher.
- Not counted: the Orchestrator's own tokens (main session) and tasks 000 to 012 (only rough "about Nk" notes in their results).
- Agent models: cpp opus, bp sonnet, level-design sonnet, orchestrator opus (`.claude/agents/`).
- Each finished subtask adds a row here ([[orchestration-protocol]] step 5).

## Per subtask
| Subtask | Model | Tokens | Ceiling | Runs |
|---|---|---|---|---|
| [[task-013-01-baseline]] | opus | 110k | 90k | 1 |
| [[task-013-02-final]] | opus | 125k | 80k | 1 |
| [[task-014-01-spawn-packs]] | opus | 96k | 80k | 1 |
| [[task-014-02-upgrade-system]] | opus | 162k | 120k | 1 |
| [[task-014-03-levelup-flow]] | opus | 151k | 90k | 1 |
| [[task-014-04-bp-upgrade-ui]] | sonnet | 244k | 110k | 4 |
| [[task-014-05-tests-integration]] | opus | 174k | 70k | 2 |
| [[task-015-01-shotgun-cpp]] | opus | 203k | 110k | 2 |
| [[task-015-02-bp-shotgun]] | sonnet | 123k | 90k | 2 |
| [[task-016-01-alert-console]] | opus | 253k | 130k | 2 |
| [[task-016-02-shuttle]] | opus | 126k | 90k | 1 |
| [[task-016-03-bp-objectives]] | sonnet | 204k | 120k | 4 |
| [[task-016-04-tests-integration]] | opus | 74k | 70k | 2 |
| [[task-017-01-radius]] | opus | 51k | 50k | 1 |
| [[task-018-01-trace]] | opus | 91k | 90k | 1 |
| [[task-018-02-crowd-collision]] | opus | 124k | 90k | 1 |
| [[task-018-03-light-movement]] | opus | 101k | 90k | 1 |
| [[task-018-04-hitch]] | opus | 77k | 90k | 1 |
| [[task-018-05-final-stress]] | opus | 56k | 70k | 1 |

## Totals
| Task | Tokens |
|---|---|
| [[task-013-stress-test]] | 235k |
| [[task-014-upgrades-levelup]] | 826k |
| [[task-015-shotgun]] | 326k |
| [[task-016-console-extraction]] | 657k |
| [[task-017-kamikaze-radius]] | 51k |
| [[task-018-robot-perf]] | 450k |
| **All** | **2546k** |

By model: opus 1975k, sonnet 571k.

## Lessons for estimates
- C++ subtasks run 50k to 250k; a typical feature subtask with tests is 100k to 200k, often over its ceiling.
- Blueprint subtasks through Unreal MCP needed 2 to 4 runs (turn limit, screenshots overflowing context, graph tools failing), about 120k to 245k each: budget them like C++ subtasks.
- Measurement subtasks (stress runs, traces) stay near 55k to 125k.

## Relations
- [[production-index]] — parent index.
- [[orchestration-protocol]] — step 5 records each subtask here.
- [[tasks-index]] — the tasks measured.
