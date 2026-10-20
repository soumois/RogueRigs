---
id: task-254-02-drone-thruster-particles
description: "Two Niagara components on the drone fed with its speed."
parent: task-254-drone-thruster-particles
kind: task
status: in-progress
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
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-254-drone-thruster-particles]] — parent global task.
