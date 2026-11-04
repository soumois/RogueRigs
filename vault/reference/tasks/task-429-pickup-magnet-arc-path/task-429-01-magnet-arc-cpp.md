---
id: task-429-01-magnet-arc-cpp
description: "Cpp: ARRPickup magnet flight along an arc (bArcMagnet and arc tunables)"
parent: task-429-pickup-magnet-arc-path
kind: task
status: done
---
# Pickup magnet flight along an arc: cpp

- goal: Implement the global goal in ARRPickup (RRPickup.h/.cpp; URRPickupSubsystem only if needed): bArcMagnet and every arc tunable listed in the global goal (angle min/max, straighten exponent, side mode, hop switch and hop height) with the decided values as C++ defaults, the vertical hop only for a latch from Rest (on the ground), header comments citing task-429 and owner 2026-11-04 provisional, per-latch random draws, curved FlyToMiner step that converges on a moving miner. Keep the speed profile, CollectDistance, heal refusal, mid-air latch and stream behaviour. No new test (not requested); fix only existing test expectations this change breaks.
- agent: cpp
- depends_on: none
- inputs: Source/RogueRigs/Pickups/RRPickup.h, Source/RogueRigs/Pickups/RRPickup.cpp, Source/RogueRigs/Pickups/RRPickupSubsystem.cpp, agent-cpp (vault/reference/agents/agent-cpp.md), sys-pickups (vault/reference/tech/systems/world/sys-pickups.md), sys-pickups-lifecycle (vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), task-429-pickup-magnet-arc-path (vault/reference/tasks/task-429-pickup-magnet-arc-path.md) (vault/reference/tasks/task-429-pickup-magnet-arc-path.md) (global request, goal and decided Questions)
- expected_output: RRPickup.h/.cpp changed and committed; change summary (property names, defaults, method, changed functions, adjusted tests if any, commit hash) for the doc subtask.
- done_when: The editor target builds without error; the existing pickup automation tests (RogueRigs.Pickups, RogueRigs.PickupPool, RogueRigs.PulseDesync) pass; a latch from Rest adds the hop (peak about ArcHopHeight at mid-flight), a mid-air latch has none; with bArcMagnet true the magnet step deviates from the direct line by the drawn angle scaled by remaining / latch distance and still reaches CollectDistance of a moving miner; with bArcMagnet false the code path equals the straight-line magnet.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: opus
- tokens: input 34, cache write 114536, cache read 1279065, output 26941
- usage: turns 17, context start 12k peak 115k, resumes 0, 7 min, units 0.41 M
- runs: 1
- milestone: M2
- started: 2026-11-04T20:36:53Z
- waiting_for: none
- result: Done, commit de1882b2: ARRPickup arc magnet (bArcMagnet, ArcAngleMin/Max 25-50, ArcStraighten 1, ArcSideMode, bArcHopFromGround, ArcHopHeight 80); build OK, 15/15 pickup tests pass, none adjusted. Path length +30 percent only estimated, not measured.

## Relations
- [[task-429-pickup-magnet-arc-path]] — parent global task.
