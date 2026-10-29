---
id: task-371-02-doc-extractor-arrival
description: "Doc: update sys-objectives-shuttle and sys-run-transition-fade for the arrival extractor."
parent: task-371-extractor-arrival
kind: task
status: done
---
# Extractor: drops the miner and takes off at level start: doc

- goal: The docs describe the arrival placement, mode and takeoff trigger.
- agent: doc
- depends_on: task-371-01-cpp-extractor-arrival
- inputs: agent-doc (vault/reference/agents/agent-doc.md), sys-objectives-shuttle (vault/reference/tech/systems/world/sys-objectives/sys-objectives-shuttle.md), sys-run-transition-fade (vault/reference/tech/systems/world/sys-run/sys-run-transition-fade.md)
- expected_output: nodes updated; one commit.
- done_when: arrival placement, mode, trigger and log line documented with the commit hash; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 12, cache write 29672, cache read 102832, output 3685
- usage: turns 6, context start 12k peak 30k, resumes 0, 1 min, units 0.07 M
- runs: 1
- milestone: M2
- started: 2026-10-29T21:41:05Z
- waiting_for: none
- result: Done, commit 1a58c95c: arrival placement, mode, trigger, log lines documented in sys-objectives-shuttle, sys-objectives-run-flow, sys-run-transition-fade, sys-player; lint clean.

## Relations
- [[task-371-extractor-arrival]] — parent global task.
