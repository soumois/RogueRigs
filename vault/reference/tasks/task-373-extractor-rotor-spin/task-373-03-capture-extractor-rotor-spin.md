---
id: task-373-03-capture-extractor-rotor-spin
description: "Capture: -game capture of the rotors spinning on the ground and in flight (Q8)."
parent: task-373-extractor-rotor-spin
kind: task
status: todo
---
# Extractor: the four rotors spin: capture

- goal: Frames show the rotors turning.
- agent: capture
- depends_on: task-373-01-cpp-extractor-rotor-spin
- inputs: agent-capture (vault/reference/agents/agent-capture.md), tool-capture (vault/reference/tech/pipelines/tool-capture.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), orchestrator-lessons (vault/reference/agents/orchestrator-lessons.md)
- expected_output: frames and a per-point report in Saved/Captures/task373/.
- done_when: C1 on consecutive frames of the landed extractor the blades of each rotor are at different angles; C2 same during the takeoff; C3 the body does not turn; report written.
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
- [[task-373-extractor-rotor-spin]] — parent global task.
