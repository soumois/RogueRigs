---
id: task-372-03-capture-extractor-footprint-clear
description: "Capture: -game capture of an arrival footprint cleared (Q8)."
parent: task-372-extractor-clears-rocks
kind: task
status: todo
---
# Extractor: blocks under its footprint are destroyed, resources stay: capture

- goal: Frames show no rock under the extractor and any resources left on the floor.
- agent: capture
- depends_on: task-372-01-cpp-extractor-footprint-clear
- inputs: agent-capture (vault/reference/agents/agent-capture.md), tool-capture (vault/reference/tech/pipelines/tool-capture.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), orchestrator-lessons (vault/reference/agents/orchestrator-lessons.md)
- expected_output: frames and a per-point report in Saved/Captures/task372/.
- done_when: C1 no rock is drawn under or through the arrival extractor; C2 where the log counts a broken deposit, its Duskite chunks lie on the floor after the takeoff (not applicable when the counts are 0, reported); report written.
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
- [[task-372-extractor-clears-rocks]] — parent global task.
