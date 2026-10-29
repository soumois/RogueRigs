---
id: task-370-03-capture-extractor-takeoff
description: "Capture: -game capture of the end-screen takeoff (Q8)."
parent: task-370-extractor-takeoff-exit
kind: task
status: todo
---
# Extractor: helicopter takeoff behind the end screen: capture

- goal: Frames show the miner gone and the extractor taking off behind the end screen.
- agent: capture
- depends_on: task-370-01-cpp-extractor-takeoff
- inputs: agent-capture (vault/reference/agents/agent-capture.md), tool-capture (vault/reference/tech/pipelines/tool-capture.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), orchestrator-lessons (vault/reference/agents/orchestrator-lessons.md)
- expected_output: frames and a per-point report in Saved/Captures/task370/.
- done_when: C1 the end screen is up and the miner and drone are not drawn; C2 behind it the extractor first rises in place; C3 it then tilts nose down and moves off along its nose; C4 it leaves the camera view; C5 robots stay frozen; report written.
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
- [[task-370-extractor-takeoff-exit]] — parent global task.
