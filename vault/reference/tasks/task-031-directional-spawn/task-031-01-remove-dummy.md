---
id: task-031-01-remove-dummy
description: "Target dummy removed: C++ class, Blueprint and material if unreferenced, docs; tests that used it switch to a still chaser."
parent: task-031-directional-spawn
kind: task
status: done
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
- model: opus
- tokens: input 124, cache write 155085, cache read 5631802, output 37801
- runs: 2
- milestone: M2
- result: RRTargetDummy.h/.cpp, BP_TargetDummy and MI_GB_Dummy removed (no other reference in Content or Config); tests use RRTestHarness::SpawnStillChaser (native chaser, tick and movement off, 1000 hp, feet snapped to the floor); Weapons.DummyRefill removed; sys-target-dummy and bp-target-dummy removed, links in old task nodes made plain text. Build 0, 185/185. Commit 19c66ff.

## Relations
- [[task-031-directional-spawn]] — parent global task.
