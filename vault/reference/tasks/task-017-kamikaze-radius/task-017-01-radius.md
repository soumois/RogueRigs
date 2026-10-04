---
id: task-017-01-radius
description: "Set the kamikaze explosion radius to 312 uu in data, make sure the explosion visual scales with it, update tests and docs."
parent: task-017-kamikaze-radius
kind: task
status: in-progress
---
# Kamikaze radius 312 uu

- goal: the kamikaze explodes at 312 uu.
- agent: cpp
- depends_on: none
- inputs: [[enemy-kamikaze]], [[explosions]], [[sys-explosions]], [[sys-enemies]], [[bp-kamikaze]], [[bp-explosion-fx]], [[sys-automation-tests]], [[cpp-standard]].
- expected_output: kamikaze radius 312 in its stats data (CSV or Blueprint default, wherever it lives; if it is a Blueprint default, report it for the bp agent instead of editing the asset); the explosion FX scale derived from the radius (confirm, or report what the bp agent must change); tests that assumed 250 updated and one asserting a target at about 300 uu is hit; [[sys-explosions]], [[sys-enemies]] updated.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 50000
- milestone: M1
- result: open

## Relations
- [[task-017-kamikaze-radius]] — parent global task.
