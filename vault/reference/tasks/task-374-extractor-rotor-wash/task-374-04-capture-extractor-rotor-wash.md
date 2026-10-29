---
id: task-374-04-capture-extractor-rotor-wash
description: "Capture: -game capture of the wash on the ground and during takeoff (Q8)."
parent: task-374-extractor-rotor-wash
kind: task
status: todo
---
# Extractor: rotor-wash effect under the four rotors: capture

- goal: Frames show the wash under the four rotors.
- agent: capture
- depends_on: task-374-02-bp-shuttle-rotor-wash-wiring
- inputs: agent-capture (vault/reference/agents/agent-capture.md), tool-capture (vault/reference/tech/pipelines/tool-capture.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), orchestrator-lessons (vault/reference/agents/orchestrator-lessons.md)
- expected_output: frames and a per-point report in Saved/Captures/task374/.
- done_when: C1 a wash is visible under each of the four rotors of the landed extractor; C2 a dust ring spreads on the floor under it; C3 the ring fades as the extractor climbs during takeoff; C4 the miner, robots and pickups near it stay readable; report written.
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
- [[task-374-extractor-rotor-wash]] — parent global task.
