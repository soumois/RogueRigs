---
id: task-011-01-feel-tweaks
description: "Camera arm 1700 uu without lag, block shake 25 uu at 8 Hz plus 8 uu vertical, chaser wind-up 0.1 s, tests updated."
parent: task-011-explosions
kind: task
status: in-progress
---
# Feel tweaks

- goal: the owner tweaks in code defaults and data.
- agent: cpp
- depends_on: none
- inputs: [[sys-player]], [[sys-grid]], [[sys-enemies]], [[enemy-chaser]], [[visual-style]], [[sys-automation-tests]], [[cpp-standard]].
- expected_output: `ARRMiner`: `CameraArmLength` default 1700; camera lag off (editable `bCameraLag`, default false) so the miner stays centered. `ARRArenaGrid`: `ShakeAmplitude` 25 uu, `ShakeFrequency` 8 Hz, new editable `ShakeVerticalAmplitude` 8 uu (vertical bob in the same jitter). `EnemyStats.csv` row `Chaser` WindUp 0.1. Check that `BP_Miner` and `BP_ArenaGrid` do not override these properties (binary name-table check as in task-008-02); report any override. Update tests that hard-code the old values (chaser attack timing). Docs [[sys-player]], [[sys-grid]], [[sys-enemies]], [[sys-automation-tests]].
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs match; lint clean.
- editor_state: closed
- token_ceiling: 50000
- milestone: M1
- result: open

## Relations
- [[task-011-explosions]] — parent global task.
