---
id: task-254-02-drone-thruster-particles
description: "Two Niagara components on the drone fed with its speed."
parent: task-254-drone-thruster-particles
kind: task
status: done
---
# Drone thruster wiring

- goal: Two Niagara components on the drone fed with its speed.
- agent: cpp
- depends_on: task-254-01-drone-thruster-particles, task-253-01-drone-parts-animation
- inputs: drone (vault/reference/gdd/player/drone.md), sys-drone (vault/reference/tech/systems/combat/sys-drone.md), sys-drone-dig (vault/reference/tech/systems/combat/sys-drone/sys-drone-dig.md), bp-dig-impact (vault/reference/tech/blueprints/fx/bp-dig-impact.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md)
- expected_output: C++: two UNiagaraComponent members (left and right pod, offsets EditDefaultsOnly, constructor reference to NS_DroneThruster like the dig impact), User.Speed written each tick from the drone speed (0 at rest), never active under -nullrhi; docs updated.
- done_when: Build passes; existing drone tests pass (HealVein excluded); no new test; docs updated; lint clean.
- editor_state: closed (C++ build)
- token_ceiling: 150k
- model: opus
- tokens: input 56, cache write 78706, cache read 1453587, output 13810
- runs: 1
- milestone: M1
- result: ThrusterLeft and ThrusterRight (UNiagaraComponent) on ARRDrone, attached to the base, NS_DroneThruster by constructor reference, User.Speed written each tick from the actor's own speed (smoothed 0.1 s), active in every state except under -nullrhi; offsets (0, +-27, -17) provisional. Build OK, 25 tests pass (HealVein known). Look and runtime drive unverified.

## Relations
- [[task-254-drone-thruster-particles]] — parent global task.
