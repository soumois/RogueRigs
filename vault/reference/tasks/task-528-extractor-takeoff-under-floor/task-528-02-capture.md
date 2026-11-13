---
id: task-528-02-capture
description: "Capture: arrival takeoff followed by the miner"
parent: task-528-extractor-takeoff-under-floor
kind: task
status: todo
---
# L'extracteur rentre dans le sol en partant: capture

- goal: arrival takeoff followed by the miner
- agent: capture
- depends_on: task-528-01-takeoff-path
- inputs: agent-capture (vault/reference/agents/agent-capture.md), extraction (vault/reference/gdd/run/extraction.md)
- expected_output: see done_when
- done_when: C1 frames over the whole takeoff, the extractor always above the floor; C2 it leaves the view climbing; frames sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-528-extractor-takeoff-under-floor]] — parent global task.
