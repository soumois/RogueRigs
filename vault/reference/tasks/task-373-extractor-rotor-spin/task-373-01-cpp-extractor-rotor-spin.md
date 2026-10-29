---
id: task-373-01-cpp-extractor-rotor-spin
description: "Cpp: spin the four rotor components per Q6, ground and flight speeds, ramp, ticking while paused; speed fraction getter and event."
parent: task-373-extractor-rotor-spin
kind: task
status: in-progress
---
# Extractor: the four rotors spin: cpp

- goal: ARRShuttle spins its four rotor components per Q6 and reports the speed fraction.
- agent: cpp
- depends_on: task-370-01-cpp-extractor-takeoff
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), sys-objectives-shuttle (vault/reference/tech/systems/world/sys-objectives/sys-objectives-shuttle.md), extraction (vault/reference/gdd/run/extraction.md), art-prompts-extractor (vault/reference/gdd/art-audio/art-prompts-extractor.md)
- expected_output: tunables GroundRevPerSecond 3, FlightRevPerSecond 5, RampSeconds 0.5, per-rotor direction; rotation about each rotor local Z; GetRotorSpeedAlpha (pure) and BlueprintImplementableEvent OnRotorSpeedChanged(Alpha) fired while the speed ramps; one commit; change summary.
- done_when: build succeeds; a -game log samples rotor yaw over 1 s on the ground and in flight matching 3 and 5 rev/s within 5 %, directions per Q6, still turning while paused after the extraction; existing tests pass; committed by path.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-29T21:59:48Z
- waiting_for: none
- result: open

## Relations
- [[task-373-extractor-rotor-spin]] — parent global task.
