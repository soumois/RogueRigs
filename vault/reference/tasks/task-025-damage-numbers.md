---
id: task-025-damage-numbers
description: "Global task: pooled Niagara damage numbers fed by the damage pipeline, merged per robot, bigger white-gold crits, toggle in settings."
parent: tasks-index
kind: task
status: in-progress
---
# Damage numbers at horde scale

- request: Owner answers to the DRGS improvement plan (2026-10-05): "1. oui 2. Oui tout à fait, même si pour le proto, nous aurons que 3 armes maxi pour l'instant. 3. Le bouton, c'est trés important ça ! C'était une frustration pour moi 4. On peut garder le coup au sol, il faut juste que la zone soit grande alors 5. Oui, je pense que c'est important de garder un rythme soutenu de level up au départ. Aussi, essaye de comprendre la maniére dont l'xp et les minerais sont ejectés et puis \"aspirer\". [...] Je te laisse lancer toutes les tasks, mais lorsque tu as fini le stress test, je veux bien directement un rapport."
- goal: every hit shows a number like DRGS at horde scale; crits are bigger and white-gold; numbers merge per robot within 0.25 s; the player can turn them off.
- acceptance: (1) build exits 0; tests pass; (2) merge window, crit flag, toggle (tests); (3) 400 robots under fire: no frame cost regression over 0.5 ms vs the actor version (measured); (4) PIE readable; (5) docs; lint clean; committed.
- gdd_check: Read [[damage-numbers]], [[color-code]], [[sys-damage]]. Merging window open → 0.25 s (provisional). Analysis in [[drgs-analysis]]. The owner launched all lots and delegated provisional values (2026-10-05); decisions written into the GDD before planning; the grid-size lot waits for the owner after the stress report.
- confirmed_by_owner: yes (2026-10-05, all lots launched by the owner)
- milestone: M2
- next_step: task-025-01-numbers-pipeline
- result: open

## Subtasks
1. task-025-01-numbers-pipeline — cpp: damage events to a pooled number system, merge, crit, toggle, tests
2. task-025-02-bp-numbers — bp: Niagara digit-atlas system, crit pop, PIE
3. task-025-03-numbers-perf — cpp: integration test and perf check at 400 robots

## Relations
- [[tasks-index]] — parent index.
- [[task-025-01-numbers-pipeline]] — step 1, cpp.
- [[task-025-02-bp-numbers]] — step 2, bp.
- [[task-025-03-numbers-perf]] — step 3, cpp.
