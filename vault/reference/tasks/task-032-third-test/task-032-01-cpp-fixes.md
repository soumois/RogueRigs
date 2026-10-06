---
id: task-032-01-cpp-fixes
description: "Robot recycling, overtime from 3:30, speeds −10 %, faster heavy, upgrade draw weights, weapon cards every 5 levels, English C++ texts, locator target getter; tests."
parent: task-032-third-test
kind: task
status: in-progress
---
# Third test: C++ fixes

- goal: the C++ side of the owner's third play test.
- agent: cpp
- depends_on: none
- inputs: [[enemies-index]], [[enemy-heavy]], [[difficulty]], [[upgrades]], [[objective-console]], [[extraction]], [[readability-rules]], [[sys-spawner]], [[sys-spawner-director]], [[sys-upgrades]], [[sys-objectives]], [[sys-hud]], [[sys-automation-tests]], [[cpp-standard]]
- expected_output: (1) recycling: a robot not on screen and more than 2500 uu from the miner for 4 s is removed without XP or death effects and frees its alive slot (values editable); test: a miner running for 3+ min keeps getting spawns in its front sector; (2) overtime from 3:30 of level time: a horde every 30 s, wave interval halved at each overtime minute (editable); test; (3) EnemyStats.csv MoveSpeed chaser 182, shield chaser 182, kamikaze 151, shooter 151, heavy 101; heavy WindUp 0.6, one slam every 2 s (AttackRate 0.5); tests re-measured; (4) upgrade draw weights per stat (damage, fire rate, reload 3; others 1) applied before rarity, data-driven; (5) new-weapon cards only on levels multiple of 5: 1 card, 2 when at least 2 catalogue weapons remain; none on other levels (replaces the 30 % per slot rule); tests; (6) every C++ / CSV on-screen text in English (banner "HORDE INCOMING!", card and stat names, end screens, objective texts, weapon names in WeaponStats.csv); list the Blueprint texts the bp agent must translate (exact asset and widget); (7) HUD locator support: URRHUDWidget getter or event giving the current objective target (console until hacked, then shuttle) world position and whether it is on screen, plus the screen-edge point and angle for an arrow; test; docs sys-spawner, sys-spawner-director, sys-upgrades, sys-objectives, sys-hud, sys-enemies, sys-automation-tests.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 140000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-032-third-test]] — parent global task.
