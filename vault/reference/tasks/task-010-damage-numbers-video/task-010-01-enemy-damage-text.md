---
id: task-010-01-enemy-damage-text
description: "ARREnemy spawns the existing ARRFloatingDamageText above its capsule on each hit, with the dummy's colors and sizes."
parent: task-010-damage-numbers-video
kind: task
status: done
---
# Floating damage text on enemies

- goal: hit numbers above every robot, quickly, reusing existing code.
- agent: cpp
- depends_on: none
- inputs: [[sys-enemies]], [[sys-target-dummy]], [[sys-damage]], [[damage-numbers]], [[sys-automation-tests]], [[cpp-standard]].
- expected_output: `ARREnemy` binds `URRHealthAttributeSet::OnDamageReceived` and, for each hit (also the killing one), spawns `ARRFloatingDamageText` at the capsule top + an editable offset (default 40 uu) with a small random jitter, calling `InitDamageText`; editable on `ARREnemy` (defaults copied from `ARRTargetDummy`): `FloatingTextClass`, `NormalTextColor` white, `CriticalTextColor` (1, 0.85, 0.45), `NormalTextSize` 60, `CriticalTextSize` 90, `bShowDamageNumbers` true. Do not refactor the dummy now (shared component is a later task). Test `RogueRigs.Enemies.DamageNumberSpawned`: a flat-damage hit on a `BP_Chaser` spawns exactly one `ARRFloatingDamageText` above its capsule top. [[sys-enemies]] updated.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; doc matches; lint clean.
- editor_state: closed
- token_ceiling: 40000
- milestone: M1
- result: ARREnemy binds OnDamageReceived and spawns ARRFloatingDamageText at capsule top + TextSpawnHeight 40 uu (jitter 25), dummy colors and sizes, bShowDamageNumbers; killing hit included. Test Enemies.DamageNumberSpawned (text 40 uu above the capsule top). Build exit 0, 36/36 (orchestrator re-ran). Docs sys-enemies, sys-automation-tests. About 58k tokens.

## Relations
- [[task-010-damage-numbers-video]] — parent global task.
