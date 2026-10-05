---
id: task-028-balance
description: "Global task: bigger heavy slam area, shield chasers, doubled upgrade values, faster early level-ups tuned on a measured run."
parent: tasks-index
kind: task
status: todo
---
# Balance and enemies

- request: Owner answers to the DRGS improvement plan (2026-10-05): "1. oui 2. Oui tout à fait, même si pour le proto, nous aurons que 3 armes maxi pour l'instant. 3. Le bouton, c'est trés important ça ! C'était une frustration pour moi 4. On peut garder le coup au sol, il faut juste que la zone soit grande alors 5. Oui, je pense que c'est important de garder un rythme soutenu de level up au départ. Aussi, essaye de comprendre la maniére dont l'xp et les minerais sont ejectés et puis \"aspirer\". [...] Je te laisse lancer toutes les tasks, mais lorsque tu as fini le stress test, je veux bien directement un rapport."
- goal: the heavy slam covers a large area; shield chasers join the packs; upgrades give DRGS-like values; level 2 comes at about 30 s, level 5 at about 1:40, level 10 at about 3:10.
- acceptance: (1) build exits 0; tests pass; (2) heavy radius, shield behavior, upgrade values (tests); (3) measured level times in a scripted -game run within ±20 % of the targets; (4) PIE look; (5) docs; lint clean; committed.
- gdd_check: Read [[enemy-heavy]], [[enemy-chaser]], [[upgrades]], [[rarities]], [[xp-level-up]], [[color-code]]. Analysis in [[drgs-analysis]]. The owner launched all lots and delegated provisional values (2026-10-05); decisions written into the GDD before planning; the grid-size lot waits for the owner after the stress report.
- confirmed_by_owner: yes (2026-10-05, all lots launched by the owner)
- milestone: M2
- next_step: task-028-01-heavy-shield-values
- result: open

## Subtasks
1. task-028-01-heavy-shield-values — cpp: heavy slam radius 400, shield chaser, new upgrade values, tests
2. task-028-02-bp-shield-heavy — bp: shield chaser Blueprint, heavy ring at the new radius, spawner weights, PIE
3. task-028-03-xp-pacing — cpp: measure level times in -game, tune the XP curve, tests

## Relations
- [[tasks-index]] — parent index.
- [[task-028-01-heavy-shield-values]] — step 1, cpp.
- [[task-028-02-bp-shield-heavy]] — step 2, bp.
- [[task-028-03-xp-pacing]] — step 3, cpp.
