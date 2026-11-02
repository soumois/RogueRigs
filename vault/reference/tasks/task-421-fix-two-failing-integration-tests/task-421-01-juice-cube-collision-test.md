---
id: task-421-01-juice-cube-collision-test
description: "Cpp: JuiceKillDropsStreamingCubes accepts the physics-only PickupMesh (profile Pickup), other components NoCollision"
parent: task-421-fix-two-failing-integration-tests
kind: task
status: in-progress
---
# Fix the two pre-existing failing integration tests: cpp

- goal: In `Source/RogueRigs/Tests/RRJuiceIntegrationTests.cpp` (check near line 404-410, the per-primitive loop of `Integration.JuiceKillDropsStreamingCubes`), replace the "no collision" check for the cube root PickupMesh by: collision profile name == `RRCollisionChannels::PickupProfile` (`Pickup`), collision enabled == PhysicsOnly, response to ECC_Pawn == Ignore (the miner is never blocked, task-028-03 intent kept). Every other primitive keeps NoCollision; every primitive keeps CanCharacterStepUpOn == ECB_No. Identify PickupMesh as the actor root / the component named PickupMesh, not by index. Decided values: global request and gdd_check (Q1). Do not change BP_PickupXP, RRPickup or the collision config.
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-tests-juice-damage-numbers (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-juice-damage-numbers.md), sys-pickups-lifecycle (vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), `Source/RogueRigs/Tests/RRJuiceIntegrationTests.cpp`, `Source/RogueRigs/Pickups/RRPickup.cpp` (BeginPlay, ConfigurePhysicsBody: read only), `Source/RogueRigs/Projectiles/RRCollisionChannels.h` (read only)
- expected_output: RRJuiceIntegrationTests.cpp changed, one commit; change summary (hash, lines changed, what is checked now) for the doc subtask.
- done_when: Build ok; `RogueRigs.Integration.JuiceKillDropsStreamingCubes` passes headless and the other tests of RRJuiceIntegrationTests.cpp still pass; only that test file changed; committed.
- editor_state: closed
- token_ceiling: 0.6 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: 2026-11-02T23:11:02Z
- waiting_for: none
- result: open

## Relations
- [[task-421-fix-two-failing-integration-tests]] — parent global task.
