---
id: task-371-03-capture-extractor-arrival
description: "Capture: -game capture of a level 2 arrival and a level 1 start (Q8)."
parent: task-371-extractor-arrival
kind: task
status: todo
---
# Extractor: drops the miner and takes off at level start: capture

- goal: Frames show the miner beside the extractor after the fade and its takeoff.
- agent: capture
- depends_on: task-371-01-cpp-extractor-arrival
- inputs: agent-capture (vault/reference/agents/agent-capture.md), tool-capture (vault/reference/tech/pipelines/tool-capture.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), orchestrator-lessons (vault/reference/agents/orchestrator-lessons.md)
- expected_output: frames and a per-point report in Saved/Captures/task371/.
- done_when: C1 after the level 2 fade in the miner stands outside the extractor, next to it; C2 the extractor takes off right after the fade and leaves the view; C3 on a level 1 start it takes off at once; C4 the miner moves during the takeoff; report written.
- editor_state: closed
- token_ceiling: 0.4 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-371-extractor-arrival]] — parent global task.
