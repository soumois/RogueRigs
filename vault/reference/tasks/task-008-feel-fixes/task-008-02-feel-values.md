---
id: task-008-02-feel-values
description: "Pickups rest 1 s before attraction, block shake 15 uu at 10 Hz, damage text sizes 60 and 90 with a dark outline for readability."
parent: task-008-feel-fixes
kind: task
status: in-progress
---
# Feel values

- goal: the owner's feel feedback applied in code defaults.
- agent: cpp
- depends_on: task-008-01-shot-height-movement
- inputs: [[sys-pickups]], [[sys-grid]], [[sys-target-dummy]], [[damage-numbers]], [[color-code]], [[sys-automation-tests]], [[cpp-standard]].
- expected_output: `ARRPickup`: editable `AttractDelay` (default 1.0 s, counted after the pop ends) before it may fly to the miner. `ARRArenaGrid`: `ShakeAmplitude` default 15 uu, `ShakeFrequency` 10 Hz (check `BP_ArenaGrid` does not override them; if it does, report so the BP step resets them). Floating damage text: default sizes normal 60, crit 90 on `ARRTargetDummy`; a dark outline or drop shadow (e.g. a second text render in near-black, slightly larger or offset behind, fading together), colors unchanged (white, white-gold). Tests: `Dig.PickupCollect` updated for the delay (not collected before 1 s + pop, collected after). Docs [[sys-pickups]], [[sys-grid]], [[sys-target-dummy]] updated.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs match; lint clean.
- editor_state: closed
- token_ceiling: 60000
- milestone: M1
- result: open

## Relations
- [[task-008-feel-fixes]] — parent global task.
