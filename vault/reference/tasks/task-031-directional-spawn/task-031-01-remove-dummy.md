---
id: task-031-01-remove-dummy
description: "Target dummy removed: C++ class, Blueprint and material if unreferenced, docs; tests that used it switch to a still chaser."
parent: task-031-directional-spawn
kind: task
status: in-progress
---
# Remove the target dummy

- goal: no immobile test enemy left in the game.
- agent: cpp
- depends_on: none
- inputs: `sys-target-dummy` (removed in task-031-01), `bp-target-dummy` (removed in task-031-01), [[sys-automation-tests]], [[cpp-standard]]
- expected_output: delete Source/RogueRigs/Debug/RRTargetDummy.h/.cpp and every reference (RRDamageNumberSubsystem.h, RRTestAccess.h, tests in RRDamageNumberTests, RRExplosionTests, RRWeaponTests: use a chaser with movement and attacks disabled through a test helper); check that BP_TargetDummy and MI_GB_Dummy are referenced by no other asset or map (search their asset path strings in Content and Config), then git rm them; remove sys-target-dummy and bp-target-dummy from the vault with their relations (indexes and any parent), and fix the links that pointed to them.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-031-directional-spawn]] — parent global task.
