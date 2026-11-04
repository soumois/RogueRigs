---
id: task-429-01-magnet-arc-cpp
description: "Cpp: ARRPickup magnet flight along an arc (bArcMagnet and arc tunables)"
parent: task-429-pickup-magnet-arc-path
kind: task
status: todo
---
# Pickup magnet flight along an arc: cpp

- goal: Implement the global goal in ARRPickup (RRPickup.h/.cpp; URRPickupSubsystem only if needed): bArcMagnet and arc tunables with the decided Q1-Q4 values as C++ defaults, header comments citing task-429 and owner 2026-11-04 provisional, per-latch random draws, curved FlyToMiner step that converges on a moving miner. Keep the speed profile, CollectDistance, heal refusal, mid-air latch and stream behaviour. No new test (not requested); fix only existing test expectations this change breaks.
- agent: cpp
- depends_on: none
- inputs: Source/RogueRigs/Pickups/RRPickup.h, Source/RogueRigs/Pickups/RRPickup.cpp, Source/RogueRigs/Pickups/RRPickupSubsystem.cpp, agent-cpp (vault/reference/agents/agent-cpp.md), sys-pickups (vault/reference/tech/systems/world/sys-pickups.md), sys-pickups-lifecycle (vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), task-429-pickup-magnet-arc-path (vault/reference/tasks/task-429-pickup-magnet-arc-path.md) (vault/reference/tasks/task-429-pickup-magnet-arc-path.md) (global request, goal and decided Questions)
- expected_output: RRPickup.h/.cpp changed and committed; change summary (property names, defaults, method, changed functions, adjusted tests if any, commit hash) for the doc subtask.
- done_when: The editor target builds without error; the existing pickup automation tests (RogueRigs.Pickups, RogueRigs.PickupPool, RogueRigs.PulseDesync) pass; with bArcMagnet true the magnet step deviates from the direct line by the drawn angle scaled by remaining / latch distance and still reaches CollectDistance of a moving miner; with bArcMagnet false the code path equals the straight-line magnet.
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
- [[task-429-pickup-magnet-arc-path]] — parent global task.
