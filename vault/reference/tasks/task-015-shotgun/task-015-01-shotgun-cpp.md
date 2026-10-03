---
id: task-015-01-shotgun-cpp
description: "Shotgun weapon: instant 60° cone, per-enemy grid line of sight, damage and crit per enemy, knockback, magazine and reload; start loadout; tests."
parent: task-015-shotgun
kind: task
status: in-progress
---
# Shotgun C++

- goal: the shotgun works in C++ and is equipped at start.
- agent: cpp
- depends_on: task-014-05-tests-integration
- inputs: [[shotgun]], [[sys-weapons]], [[sys-auto-fire]], [[sys-damage]], [[sys-grid]], [[sys-upgrades]], [[sys-enemies]], [[sys-automation-tests]], [[cpp-standard]].
- expected_output: shotgun weapon class reusing the weapon ASC and auto-fire (fire rate 0.8/s, magazine 6, reload 2 s; data in the weapon CSV); on fire: robots within 700 uu and ±30° of the aim, kept if the grid line of sight is clear (rock blocks), each takes 18 damage through the existing damage GE with its own crit roll and a knockback of 600 outward scaled by (1 − knockback resistance); an event with the cone parameters and the rock-clipped length for the VFX. Miner default loadout: burst rifle + shotgun. The upgrade offer sees the shotgun as an owned target. [[sys-weapons]], [[sys-damage]] updated. Tests `RogueRigs.Shotgun.*`: inside cone hit, outside angle or range not hit; robot behind rock not hit; several robots hit at once with independent crits (seeded); knockback direction and resistance; shotgun cards appear only when equipped.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 110000
- milestone: M1
- result: open

## Relations
- [[task-015-shotgun]] — parent global task.
