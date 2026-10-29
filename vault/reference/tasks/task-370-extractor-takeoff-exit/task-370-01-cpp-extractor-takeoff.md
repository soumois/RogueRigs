---
id: task-370-01-cpp-extractor-takeoff
description: "Cpp: hide miner and drone on extraction, helicopter takeoff of ARRShuttle ticking while paused, StartTakeoff entry and events."
parent: task-370-extractor-takeoff-exit
kind: task
status: todo
---
# Extractor: helicopter takeoff behind the end screen: cpp

- goal: ARRShuttle plays the Q3 takeoff on a successful extraction while the game is paused, with the miner and drone hidden.
- agent: cpp
- depends_on: task-369-02-cpp-shuttle-rotor-components
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), sys-objectives-shuttle (vault/reference/tech/systems/world/sys-objectives/sys-objectives-shuttle.md), sys-objectives-run-flow (vault/reference/tech/systems/world/sys-objectives/sys-objectives-run-flow.md), sys-objectives-events (vault/reference/tech/systems/world/sys-objectives/sys-objectives-events.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-player (vault/reference/tech/systems/core/sys-player.md), sys-drone (vault/reference/tech/systems/combat/sys-drone.md), extraction (vault/reference/gdd/run/extraction.md), intermission (vault/reference/gdd/run/intermission.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md)
- expected_output: ARRShuttle: StartTakeoff() (public, BlueprintCallable), takeoff state ticking while paused, tunables SpinUpSeconds 0.5, RiseHeight 400, RiseSeconds 1.5, PitchDegrees 20, forward acceleration, removal once its bounds leave the player camera view; BlueprintImplementableEvents OnTakeoffStarted and OnTakeoffFinished; extraction success hides the miner and his drone (hidden, no collision) before the screen; one commit; change summary.
- done_when: build succeeds; a -game log of rr.Run.EndLevel or a walked extraction shows takeoff start, the out-of-view frame and the removal while the run flow is paused; RogueRigs.Extraction.* and RogueRigs.RunFlow.* pass; committed by path.
- editor_state: closed
- token_ceiling: 1.2 M units
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
