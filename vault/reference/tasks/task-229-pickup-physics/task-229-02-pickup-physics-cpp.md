---
id: task-229-02-pickup-physics-cpp
description: "cpp: Pickup collision profile (floor and walls only), landing hand-off to Chaos physics with spin, magnet and pool handling, build, S7 physics cost."
parent: task-229-pickup-physics
kind: task
status: in-progress
---
# Pickup physics (C++)

- goal: ARRPickup turns into a Chaos rigid body at landing and settles physically; flight gets a light spin.
- agent: agent-cpp
- depends_on: task-229-01-pickup-simple-collisions
- inputs: `Source/RogueRigs/Pickups/RRPickup.h/.cpp`, `RRPickupSubsystem.h/.cpp`, the actor pool code; `vault/reference/tech/systems/world/sys-pickups.md`, `vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md`, `vault/reference/tech/systems/world/sys-pickups/sys-pickups-xp.md`, `vault/reference/tasks/task-229-pickup-physics.md` (owner words and defaults), `vault/reference/tech/performance/perf-leads/perf-lead-pickup-simulated-roll.md`, `vault/reference/agents/agent-cpp.md`; the grid floor and rock cell components (find how they collide: WorldStatic or another channel).
- method:
  - Collision: a `Pickup` object channel or profile in `Config/DefaultEngine.ini` that blocks only what the floor and the rock walls use, ignores pawns, robots, projectiles, other pickups and everything else; queries off where possible.
  - Flight: the kinematic pop arc is kept (floaty values untouched); add a very light spin (new EditDefaultsOnly `PopSpinDegPerSec`, small random axis, default about 90 deg/s, provisional).
  - Landing: at the end of the arc, enable SimulatePhysics on PickupMesh with the arc's landing velocity (horizontal part of the pop) and the spin as angular velocity, then let it settle; tunables EditDefaultsOnly (linear and angular damping, friction and restitution through a physical material or body settings, sleep thresholds); the rest phase starts at landing as today (AttractDelay unchanged), the glow fade target 1 at landing as today.
  - Magnet latch: disable physics (zero velocities) before the kinematic fly-in. Collected and pool release: physics off, transform reset; pool acquire starts kinematic.
  - RestOrientation: no longer applied at landing (physics decides); keep the per-life yaw at spawn if it is used for the pop.
  - Build with the editor closed; update the existing pickup tests only where this change breaks them (no new tests). Measure the S7 burst (the existing stress procedure) with physics on, compare with the last S7 numbers; write the result into the perf lead node. Update sys-pickups-lifecycle.
- done_when: build succeeds; touched tests pass; S7 cost written; docs updated; commit by path.
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-229-pickup-physics]] — parent global task.
