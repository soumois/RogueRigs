---
id: task-027-juice
description: "Global task: DRGS-like XP and ore ejection then suction, kamikaze core light before exploding, console ring gauge, shuttle chevrons, heal ring."
parent: tasks-index
kind: task
status: in-progress
---
# Readability and pickup juice

- request: Owner answers to the DRGS improvement plan (2026-10-05): "1. oui 2. Oui tout à fait, même si pour le proto, nous aurons que 3 armes maxi pour l'instant. 3. Le bouton, c'est trés important ça ! C'était une frustration pour moi 4. On peut garder le coup au sol, il faut juste que la zone soit grande alors 5. Oui, je pense que c'est important de garder un rythme soutenu de level up au départ. Aussi, essaye de comprendre la maniére dont l'xp et les minerais sont ejectés et puis \"aspirer\". [...] Je te laisse lancer toutes les tasks, mais lorsque tu as fini le stress test, je veux bien directement un rapport."
- goal: kills burst XP cubes that scatter and rest, then stream into the miner when close; broken ore and heal blocks pop chunks; the kamikaze core flares before it explodes; the console hack shows a growing ground ring; chevrons lead to the shuttle; healing shows a green ring.
- acceptance: (1) build exits 0; tests pass; (2) pickup eject, rest and magnet behavior and kamikaze telegraph timing (tests); (3) PIE look; (4) docs; lint clean; committed.
- gdd_check: Read [[xp-level-up]], [[ore-blocks]], [[heal-ore]], [[enemy-kamikaze]], [[objective-console]], [[extraction]], [[color-code]]. Analysis in [[drgs-analysis]]. The owner launched all lots and delegated provisional values (2026-10-05); decisions written into the GDD before planning; the grid-size lot waits for the owner after the stress report.
- confirmed_by_owner: yes (2026-10-05, all lots launched by the owner)
- milestone: M2
- next_step: task-027-01-pickup-motion
- result: open

## Subtasks
1. task-027-01-pickup-motion — cpp: pickup eject, rest, magnet stream, collect event; kamikaze flare state; tests
2. task-027-02-bp-juice — bp: pickup visuals, kamikaze core light, ring gauge, chevrons, heal ring, PIE
3. task-027-03-tests-integration — cpp: integration tests with the final Blueprints

## Relations
- [[tasks-index]] — parent index.
- [[task-027-01-pickup-motion]] — step 1, cpp.
- [[task-027-02-bp-juice]] — step 2, bp.
- [[task-027-03-tests-integration]] — step 3, cpp.
