---
id: task-371-01-cpp-extractor-arrival
description: "Cpp: spawn an arrival extractor next to the miner spawn and start its takeoff at the end of the fade in, or at once without fade."
parent: task-371-extractor-arrival
kind: task
status: done
---
# Extractor: drops the miner and takes off at level start: cpp

- goal: An arrival extractor (ShuttleClass, arrival mode) stands next to the miner at level start and calls StartTakeoff when the fade in ends or, without fade, when the level plays.
- agent: cpp
- depends_on: task-370-01-cpp-extractor-takeoff
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), sys-objectives-shuttle (vault/reference/tech/systems/world/sys-objectives/sys-objectives-shuttle.md), sys-objectives-run-flow (vault/reference/tech/systems/world/sys-objectives/sys-objectives-run-flow.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-run-transition-fade (vault/reference/tech/systems/world/sys-run/sys-run-transition-fade.md), sys-run (vault/reference/tech/systems/world/sys-run.md), sys-grid-api (vault/reference/tech/systems/world/sys-grid/sys-grid-api.md), sys-player (vault/reference/tech/systems/core/sys-player.md), level-flow (vault/reference/gdd/run/level-flow.md), extraction (vault/reference/gdd/run/extraction.md)
- expected_output: arrival placement (footprint 1 cell from the spawn cell, on the screen-up side from the camera yaw), arrival mode without board or countdown, takeoff trigger on OnScreenFadeFinished(FadingIn) or level start without fade; log line per arrival with cell and trigger; one commit; change summary.
- done_when: build succeeds; a -game log of rr.Run.NextLevel shows the arrival placed and its takeoff started in the fade-in finished frame; a direct level 1 start shows the takeoff at level start; the miner can move meanwhile; RogueRigs.Extraction.* and RogueRigs.RunFlow.* pass; committed by path.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: opus
- tokens: input 78, cache write 180267, cache read 4735467, output 37670
- usage: turns 39, context start 12k peak 180k, resumes 0, 11 min, units 0.89 M
- runs: 1
- milestone: M2
- started: 2026-10-29T21:29:55Z
- waiting_for: none
- result: Done, commits 66b2c5e2 and be525566: ARRShuttle::InitializeArrival/IsArrival, URRRunFlowSubsystem::PlaceArrivalShuttle, ArrivalDistanceCells (default 1), takeoff on OnScreenFadeFinished(FadingIn) or first tick; build ok, -game log and Extraction/RunFlow tests 9/9 pass; miner movement verified by logged state only.

## Relations
- [[task-371-extractor-arrival]] — parent global task.
