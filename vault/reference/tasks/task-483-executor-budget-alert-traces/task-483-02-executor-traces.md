---
id: task-483-02-executor-traces
description: "Doc: Keep executor traces in agent-usage.js under the size limit, with usage-agents.json still free of them."
parent: task-483-executor-budget-alert-traces
kind: task
status: done
---
# Budget alert at closing and executor traces: doc

- goal: Keep executor traces in agent-usage.js under the size limit, with usage-agents.json still free of them.
- agent: doc
- depends_on: task-483-01-budget-alert
- inputs: vault/scripts/agent-usage.js, vault/scripts/usage-snapshot.json, vault/scripts/usage-agents.json
- expected_output: see done_when
- done_when: A6 to A8 of the global pass on a real executor run, with the two sizes written in result; the crew-board mod still reads usage-agents.json; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-483
- usage: included in the executor run of task-483
- runs: 1
- milestone: M2
- started: 2026-11-06T22:54:19Z
- waiting_for: none
- result: agent-usage.js --agent-debug now keeps a per-turn trace for executors in usage-snapshot.json. Real run: this executor (afdb528bc528cdd36) shows in --agent-detail from a copy with no transcript. Sizes measured at closing: real trace of task-477, 113 turns, 112821 bytes (about 1 kB per turn); usage-snapshot.json 6860658 bytes with 38 executor traces (5062432 before), so the 20 MB limit leaves room for about 190 runs like 477. usage-agents.json 419132 bytes (under 4 MiB), 792 records, 0 traces; same JSON array, crew-board register.tsx reading code unchanged. Limit snapshotMaxBytes 20000000 in cost-rates.json: tested on a throwaway copy with a lower limit: 2 oldest executor traces removed, record count and token sums identical, recent traces kept; with an unreachable limit one ALERT line and no failure. Also: a snapshot without --agent-debug no longer erases a saved trace. usage-snapshot.json stays uncommitted as before (task-next.js does not commit it). Global acceptance A1 to A9 pass; no game code, Blueprint, asset or capture. This global has no level, so no budget alert on its own closing. Cost: executor run.

## Relations
- [[task-483-executor-budget-alert-traces]] — parent global task.
