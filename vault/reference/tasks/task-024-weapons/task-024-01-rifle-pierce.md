---
id: task-024-01-rifle-pierce
description: "Burst rifle bullets pierce 4 robots; piercing becomes an upgrade stat (blue +1, purple +2, yellow +3) instead of a yellow effect; tests."
parent: task-024-weapons
kind: task
status: in-progress
---
# Rifle pierce and piercing stat

- goal: bullets pierce and piercing can be upgraded.
- agent: cpp
- depends_on: none
- inputs: [[burst-rifle]], [[upgrades]], [[sys-projectiles]], [[sys-weapons]], [[sys-upgrades]], [[cpp-standard]], [[sys-automation-tests]], [[drgs-analysis]]
- expected_output: Pierce attribute on weapons (rifle base 4: a bullet damages up to 4 distinct robots, stops on rock or the 5th); upgrade value rows for piercing (blue +1, purple +2, yellow +3; not grey or green; points); the yellow piercing effect removed from the pool; docs sys-projectiles, sys-weapons, sys-upgrades. Tests `RogueRigs.Weapons.Pierce*`.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 90000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-024-weapons]] — parent global task.
