---
id: cost-log
description: "Input, cache and output tokens, model and runs per subtask from task 013, with totals; read to estimate a future project cost."
parent: production-index
kind: note
---
# Cost log

- Source: `node vault/scripts/agent-usage.js` (add `--json`, `--from <n>`). It reads this machine's Claude Code transcripts, matches each agent to its subtask by the id at the start of its description, and counts every API call once. Delegations must therefore start their description with the subtask id.
- Columns are billed separately and at different prices: input (uncached), cache write (context written to the prompt cache), cache read (context re-read from the cache on each turn, the cheapest but by far the largest), output (the most expensive per token).
- Runs: agent launches including resumes (a resume continues the same transcript).
- Not covered: tasks 000 to 012 (their agents carry no subtask id in the description).
- Each finished subtask adds a row ([[orchestration-protocol]] step 5).

## Per subtask
| Subtask | Model | Runs | Input | Cache write | Cache read | Output |
|---|---|---|---|---|---|---|
| [[task-013-01-baseline]] | opus | 1 | 0k | 108k | 3940k | 46k |
| [[task-013-02-final]] | opus | 1 | 0k | 117k | 2652k | 50k |
| [[task-014-01-spawn-packs]] | opus | 1 | 0k | 90k | 1684k | 40k |
| [[task-014-02-upgrade-system]] | opus | 1 | 0k | 154k | 3647k | 65k |
| [[task-014-03-levelup-flow]] | opus | 1 | 0k | 143k | 5033k | 59k |
| [[task-014-04-bp-upgrade-ui]] | sonnet | 4 | 0k | 242k | 19229k | 81k |
| [[task-014-05-tests-integration]] | opus | 2 | 0k | 172k | 6700k | 65k |
| [[task-015-01-shotgun-cpp]] | opus | 2 | 0k | 196k | 8481k | 72k |
| [[task-015-02-bp-shotgun]] | sonnet | 2 | 0k | 121k | 4445k | 31k |
| [[task-016-01-alert-console]] | opus | 2 | 0k | 251k | 11240k | 116k |
| [[task-016-02-shuttle]] | opus | 1 | 0k | 119k | 2610k | 48k |
| [[task-016-03-bp-objectives]] | sonnet | 4 | 0k | 202k | 16009k | 69k |
| [[task-016-04-tests-integration]] | opus | 2 | 0k | 170k | 2291k | 44k |
| [[task-017-01-radius]] | opus | 1 | 0k | 51k | 515k | 8k |
| [[task-018-01-trace]] | opus | 1 | 0k | 84k | 2444k | 29k |
| [[task-018-02-crowd-collision]] | opus | 1 | 0k | 117k | 3839k | 41k |
| [[task-018-03-light-movement]] | opus | 1 | 0k | 95k | 2968k | 37k |
| [[task-018-04-hitch]] | opus | 1 | 0k | 70k | 1694k | 20k |
| [[task-018-05-final-stress]] | opus | 1 | 0k | 162k | 538k | 17k |
| [[task-019-01-asset-manager]] | opus | 1 | 0k | 36k | 390k | 8k |
| [[task-020-01-review]] | opus | 1 | 0k | 188k | 7197k | 38k |
| [[task-020-02-refactor]] | opus | 2 | 0k | 191k | 7042k | 69k |
| [[task-020-03-refactor-cleanup]] | opus | 1 | 0k | 117k | 3705k | 40k |
| [[task-020-04-bp-check]] | sonnet | 1 | 0k | 76k | 1578k | 9k |
| [[task-021-01-shooter-cpp]] | opus | 1 | 0k | 200k | 6584k | 87k |
| [[task-021-02-bp-shooter]] | sonnet | 2 | 0k | 138k | 6637k | 39k |
| [[task-021-03-tests-integration]] | opus | 1 | 0k | 106k | 1812k | 25k |
| [[task-022-01-heavy-cpp]] | opus | 1 | 0k | 142k | 3564k | 44k |
| [[task-022-02-bp-heavy]] | sonnet | 4 | 0k | 349k | 9269k | 59k |
| [[task-022-03-tests-integration]] | opus | 2 | 0k | 127k | 2801k | 30k |
| [[task-023-01-cell-stress]] | opus | 2 | 0k | 458k | 7854k | 67k |
| [[task-024-01-rifle-pierce]] | opus | 1 | 0k | 170k | 4373k | 56k |
| [[task-024-02-shotgun-pellets]] | opus | 1 | 0k | 184k | 5599k | 79k |

## Totals
| Scope | Input | Cache write | Cache read | Output |
|---|---|---|---|---|
| [[task-013-stress-test]] | 0k | 0.22 M | 6.59 M | 96k |
| [[task-014-upgrades-levelup]] | 1k | 0.80 M | 36.29 M | 310k |
| [[task-015-shotgun]] | 0k | 0.32 M | 12.93 M | 104k |
| [[task-016-console-extraction]] | 1k | 0.74 M | 32.15 M | 277k |
| [[task-017-kamikaze-radius]] | 0k | 0.05 M | 0.52 M | 8k |
| [[task-018-robot-perf]] | 0k | 0.53 M | 11.48 M | 144k |
| model opus | 1k | 2.10 M | 60.27 M | 757k |
| model sonnet | 1k | 0.56 M | 39.68 M | 181k |
| **All agents, tasks 013 to 018** | **2k | 2.66 M | 99.96 M | 938k** |
| Orchestrator, all main sessions of this project (also covers tasks 000 to 012 and non-task chat) | 1k | 4.47 M | 150.89 M | 438k |

## Lessons for estimates
- Per C++ subtask: output 20k to 115k, cache write 50k to 250k, cache read 0.5 M to 11 M.
- Blueprint subtasks through Unreal MCP needed 2 to 4 runs and re-read the most context (cache read 4 M to 19 M): budget them at least like C++ subtasks.
- The Orchestrator's own context, re-read on every turn of a long session, costs as much cache read as all the agents together: keep sessions shorter or hand over between global tasks.

## Relations
- [[production-index]] — parent index.
- [[orchestration-protocol]] — step 5 records each subtask here.
- [[tasks-index]] — the tasks measured.
