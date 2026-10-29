---
id: task-370-02-doc-extractor-takeoff
description: "Doc: update sys-objectives-shuttle and sys-objectives-events for the takeoff and hidden miner."
parent: task-370-extractor-takeoff-exit
kind: task
status: todo
---
# Extractor: helicopter takeoff behind the end screen: doc

- goal: The system docs describe the takeoff state, its tunables and events, and the miner hiding.
- agent: doc
- depends_on: task-370-01-cpp-extractor-takeoff
- inputs: agent-doc (vault/reference/agents/agent-doc.md), sys-objectives-shuttle (vault/reference/tech/systems/world/sys-objectives/sys-objectives-shuttle.md), sys-objectives-events (vault/reference/tech/systems/world/sys-objectives/sys-objectives-events.md), sys-objectives-run-flow (vault/reference/tech/systems/world/sys-objectives/sys-objectives-run-flow.md)
- expected_output: nodes updated; one commit.
- done_when: StartTakeoff, the tunables, OnTakeoffStarted/Finished and the hide rule documented with the commit hash; lint clean; committed by path.
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
- [[task-370-extractor-takeoff-exit]] — parent global task.
