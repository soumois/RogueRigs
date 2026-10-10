---
id: task-023-grid-perf
description: "Global task: measure 100 uu and about 67 uu cells (rendering, destruction, flow field, spawn, line of sight) before choosing the grid size."
parent: tasks-done-index
kind: task
status: done
---
# Grid granularity stress test

- request: Owner answers to the DRGS improvement plan (2026-10-05): "1. oui 2. Oui tout à fait, même si pour le proto, nous aurons que 3 armes maxi pour l'instant. 3. Le bouton, c'est trés important ça ! C'était une frustration pour moi 4. On peut garder le coup au sol, il faut juste que la zone soit grande alors 5. Oui, je pense que c'est important de garder un rythme soutenu de level up au départ. Aussi, essaye de comprendre la maniére dont l'xp et les minerais sont ejectés et puis \"aspirer\". [...] Je te laisse lancer toutes les tasks, mais lorsque tu as fini le stress test, je veux bien directement un rapport."
- goal: the owner gets a report with real frame times to decide the cell size; no gameplay change.
- acceptance: (1) test arenas at 100 uu and about 67 uu cells with the same world size; (2) -game frame times at 100 and 400 robots, mass destruction and explosion chains, flow-field rebuild time, spawn scan time, memory; (3) report in a new performance node and sent to the owner; no gameplay code change.
- gdd_check: Read [[arena-grid]], [[stress-test-m1]], [[stress-opt-m1]], [[perf-leads]]. Analysis in [[drgs-analysis]]. The owner launched all lots and delegated provisional values (2026-10-05); decisions written into the GDD before planning; the grid-size lot waits for the owner after the stress report.
- confirmed_by_owner: yes (2026-10-05, all lots launched by the owner)
- milestone: M2
- next_step: none
- result: Report delivered to the owner (stress-grid-cells): 100 uu viable with a 1 h fix, 67 uu needs 17–23 h. Grid size decision pending with the owner.

## Subtasks
1. task-023-01-cell-stress — cpp: test-only smaller-cell arenas, -game measurements, report

## Relations
- [[tasks-done-index]] — parent index.
- [[task-023-01-cell-stress]] — step 1, cpp.
