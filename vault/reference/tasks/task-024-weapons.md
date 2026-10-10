---
id: task-024-weapons
description: "Global task: rifle pierces 4, shotgun pellets toward the aim, turret launcher on a button, weapons as level-up cards, piercing as a stat."
parent: tasks-done-index
kind: task
status: done
---
# Weapons rework

- request: Owner answers to the DRGS improvement plan (2026-10-05): "1. oui 2. Oui tout à fait, même si pour le proto, nous aurons que 3 armes maxi pour l'instant. 3. Le bouton, c'est trés important ça ! C'était une frustration pour moi 4. On peut garder le coup au sol, il faut juste que la zone soit grande alors 5. Oui, je pense que c'est important de garder un rythme soutenu de level up au départ. Aussi, essaye de comprendre la maniére dont l'xp et les minerais sont ejectés et puis \"aspirer\". [...] Je te laisse lancer toutes les tasks, mais lorsque tu as fini le stress test, je veux bien directement un rapport."
- goal: start with the burst rifle only; new weapons come as level-up cards (4 slots, 3 weapons in the prototype); rifle bullets pierce 4 robots; shotgun shoots 7 piercing pellets toward the aim; turret launcher places turrets with a button; piercing is an upgrade stat.
- acceptance: (1) build exits 0; all tests pass, new ones included; (2) rifle pierce 4 (test); (3) shotgun pellets: count, spread, pierce 3, knockback, rock stop, crit per pellet (tests); (4) turret: button placement, charges, lifetime, targeting, pierce (tests); (5) weapon cards: only when a slot is free, picking equips the weapon, its upgrades join the pool (tests); (6) piercing upgrade (test); (7) PIE readable; (8) docs; lint clean; committed.
- gdd_check: Read [[burst-rifle]], [[shotgun]], [[turret]], [[upgrades]], [[rarities]], [[miner]], [[damage-numbers]]. Analysis in [[drgs-analysis]]. The owner launched all lots and delegated provisional values (2026-10-05); decisions written into the GDD before planning; the grid-size lot waits for the owner after the stress report.
- confirmed_by_owner: yes (2026-10-05, all lots launched by the owner)
- milestone: M2
- next_step: none
- result: All acceptance checks passed: 142/142 tests (rifle pierce 4, shotgun pellets with pierce 3 and knockback, turret on a button, weapon cards with 4 slots, piercing stat); PIE: rifle-only start, cards equip the shotgun and launcher, E places a firing turret. To check by eye: card border, weapon slots, turret bar, muzzle flashes; IA_PlaceTurret asset not created (C++ fallback input works).

## Subtasks
1. task-024-01-rifle-pierce — cpp: rifle pierce 4, piercing as an upgrade stat, tests
2. task-024-02-shotgun-pellets — cpp: shotgun pellets toward the aim, pierce 3, knockback, tests
3. task-024-03-turret — cpp: turret launcher on a button, placed turrets, tests
4. task-024-04-weapon-cards — cpp: start with the rifle, new-weapon cards, 4 slots, tests
5. task-024-05-bp-weapons — bp: pellets, turret, weapon-card and slot visuals, PIE
6. task-024-06-tests-integration — cpp: integration tests with the final Blueprints

## Relations
- [[tasks-done-index]] — parent index.
- [[task-024-01-rifle-pierce]] — step 1, cpp.
- [[task-024-02-shotgun-pellets]] — step 2, cpp.
- [[task-024-03-turret]] — step 3, cpp.
- [[task-024-04-weapon-cards]] — step 4, cpp.
- [[task-024-05-bp-weapons]] — step 5, bp.
- [[task-024-06-tests-integration]] — step 6, cpp.
