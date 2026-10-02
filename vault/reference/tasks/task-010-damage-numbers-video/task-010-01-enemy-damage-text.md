---
id: task-010-01-enemy-damage-text
description: "ARREnemy spawns the existing ARRFloatingDamageText above its capsule on each hit, with the dummy's colors and sizes."
parent: task-010-damage-numbers-video
kind: task
status: in-progress
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
- result: open

## Relations
- [[task-010-damage-numbers-video]] — parent global task.
