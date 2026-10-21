---
id: task-267-01-crystal-pulse-restore
description: "The deposit crystals pulse again, and the heal crystals get their hashed pulse back."
parent: task-267-crystal-pulse-restore
kind: task
status: done
---
# Crystal pulse restore

- goal: The deposit crystals pulse again, and the heal crystals get their hashed pulse back.
- agent: cpp
- depends_on: none
- inputs: bp-deposit-crystal-material (vault/reference/tech/blueprints/fx/bp-deposit-crystal-material.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), bp-pickup-ore (vault/reference/tech/blueprints/fx/bp-pickup-ore.md), sys-grid-deposits (vault/reference/tech/systems/world/sys-grid/sys-grid-deposits.md), sys-pickups-lifecycle (vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md), bp-rock-slab-material (vault/reference/tech/blueprints/world/bp-rock-slab-material.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), sys-tests-pools-pickups (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-pools-pickups.md)
- expected_output: Diagnosis with evidence of why the Duskite and heal crystals no longer pulse (leads: M_XPNutGlow and its copy M_DepositCrystalGlow multiply the pulse rate by a scalar parameter read from Custom Primitive Data index 2: if the engine gives 0 for a primitive that never writes it (the instanced deposits), Time x 0 freezes the pulse; the capped emission of task-262 also lowers the swing; check the pickups and the XP nut too); C++ side of the fix: deposit pulse data moved to per-instance indices 13 and 14 (15 floats) for every deposit instanced mesh, heal ones included (the heal rock reads 4..12 as shake and must stay 0); then a bp subtask: M_DepositCrystalGlow reads 13 and 14 and drops the Custom Primitive Data parameters.
- done_when: Cause identified with numbers; PulseDesync check updated to the new indices and passing; existing tests pass; docs updated; lint clean.
- editor_state: closed (C++ build)
- token_ceiling: 150k
- model: opus
- tokens: input 102, cache write 146571, cache read 4909574, output 37956
- runs: 1
- milestone: M1
- result: Cause (engine source, 5.8): a scalar parameter with Use Custom Primitive Data compiles to the CPD slot and never uses its default; a primitive that writes nothing gets 36 zeroed floats, so PulseRate reads 0 and Time x 0 freezes the pulse: measured 0.0000 on the Duskite and heal deposit ISMs and on BP_PickupXP; ore pickups write 0.8972 and still pulse; task-262 values not the cause. XP nut frozen too (regression of 257-01). C++: deposit pulse data moved to per-instance indices 13 and 14 with 15 floats on every deposit ISM (heal rock keeps 0 at 4..12, shake input 0.0000 on 12 of 12; 371 instances hold their hash); cells keep 4 and 5; PulseDesync check updated; 26 of 26 other tests pass. Pending bp (267-02): M_DepositCrystalGlow reads 13 and 14 and drops its CPD parameters; M_XPNutGlow: If(PulseRate > 0, PulseRate, 1). Not seen on screen.

## Relations
- [[task-267-crystal-pulse-restore]] — parent global task.
