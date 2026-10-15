---
id: task-192-03-pool-reuse-test-rotation
description: "C++ test fix: ReusedPickupEqualsFresh ignores rotation for pickups with a random rest orientation (broken by task-192-02 and task-189-05)."
parent: task-192-pickup-spawn-orientation-variants
kind: task
status: in-progress
---
# Pool reuse test: random rest orientation

- goal: update the expectation of RogueRigs.PickupPool.ReusedPickupEqualsFresh (Source/RogueRigs/Tests/RRPickupScaleTests.cpp) broken by the random RestOrientation set on BP_PickupXP, BP_PickupOre and BP_PickupHeal: the actor and PickupMesh rotation checks apply only when RestOrientation is None (for YawOnly, check pitch and roll stay 0); other checks unchanged; no new test.
- agent: cpp
- depends_on: task-192-01-pickup-spawn-orientation-variants
- inputs: sys-pickups-lifecycle (vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md), sys-tests-pools-pickups (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-pools-pickups.md), sys-pickups (vault/reference/tech/systems/world/sys-pickups.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: the test file updated; sys-tests-pools-pickups updated; the gotcha added to sys-pickups by task-195-01 removed.
- done_when: build clean; RogueRigs.Pickup, RogueRigs.PickupPool and RogueRigs.ActorPool tests all pass; docs updated; lint clean; committed by path.
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-192-pickup-spawn-orientation-variants]] — parent global task.
