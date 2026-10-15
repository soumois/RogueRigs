---
id: task-192-01-pickup-spawn-orientation-variants
description: "C++: ARRPickup rest orientation mode (None, YawOnly, Full) and random mesh variant drawn once per life, pool-safe, defaults unchanged."
parent: task-192-pickup-spawn-orientation-variants
kind: task
status: todo
---
# Pickup spawn orientation and mesh variants

- goal: Pickup spawn: random rest orientation per Blueprint (none, yaw only, full) and a random mesh variant picked per drop, pool-safe.
- agent: cpp
- depends_on: none
- inputs: sys-pickups (vault/reference/tech/systems/world/sys-pickups.md), sys-pickups-lifecycle (vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md), sys-pickups-xp (vault/reference/tech/systems/world/sys-pickups/sys-pickups-xp.md), sys-actor-pool (vault/reference/tech/systems/world/sys-actor-pool.md), bp-pickup-ore (vault/reference/tech/blueprints/fx/bp-pickup-ore.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), bp-pickup-heal (vault/reference/tech/blueprints/fx/bp-pickup-heal.md), sys-tests-pools-pickups (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-pools-pickups.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: C++ in Source/RogueRigs/Pickups/: on ARRPickup an editable rest orientation mode (None, YawOnly, Full; default None so existing Blueprints keep today's look) applied once per life when the pickup is ejected (StartLife, so pooled pickups get a new draw on every OnPoolAcquired; no tumble in flight, no tick added), and an editable list of mesh variants (empty = keep PickupMesh) of which one is picked at random per life; docs sys-pickups and sys-pickups-lifecycle updated.
- done_when: editor build succeeds with no new warning; existing pickup and pool automation tests pass (only expectations a change breaks are updated, no new test); docs updated; lint clean; committed by path.
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-192-pickup-spawn-orientation-variants]] — parent global task.
