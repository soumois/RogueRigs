---
id: task-013-stress-test
description: "Global task: measure frame time against enemy count in -game, find the ceiling, list optimizations; baseline first, final run after task-016."
parent: tasks-index
kind: task
status: done
---
# Stress test M1

- request: "Concernant la task 12, je valide. J'aurais juste d'autre points, les kamikaze doit être moins nombreux que les chasseurs. Aussi, tu peux multiplier par 5, le nombre d'enemi. Mais essaye qu'ils arrivent plus en "paquet" qu'au compte goute. [...] Tu peux commencer à faire l'amélioration des upgrades, ne fait que les caractéristiques de base ( pas le nombre de balle ) et fait déjà le système de rareté. [...] Lorsque je level up , il faut un effet, une récupération de pv ( un son plus tard ) et seulement après le jeu se met en pause pour proposer l'amélioration. Pour l'instant, on aura le droit à un bouton pour refaire une sélection des propositions ( mais qu'une fois ) [...] tu peux enchainer sur le fusil à pompe,le piratage de la console et extraction de navette. Et lance un stress test toi même pour commencer [...]" Then: "rajoute le fire rate, la vitesse de rechargement, les dégats pour les améliorations"; plan confirmed "oui go" (2026-10-03).
- goal: know how many robots the game holds at 16.6 ms average and 25 ms p99 on this PC at 1920×1080, before and after the M1 features, with ranked optimization leads.
- acceptance: (1) a repeatable stress command and procedure documented in [[stress-test-m1]]; (2) a frame-time table per enemy count (S0) for the baseline and for the final build (S0, S2, S7); (3) the ceiling count and a ranked list of optimization leads with expected gain; no optimization coded without the owner.
- gdd_check: Read [[stress-test-m1]], [[perf-risks]], [[gas-scope]]. Target machine open → owner: this PC, 1920×1080. Owner decisions of 2026-10-03 written into the GDD before planning (level-up heal vs heal-ore-only, rarities moved to M1, reroll, spawn packs, all open values set as provisional defaults). TaskCreate and TaskUpdate are not available in this session: progress is tracked in these nodes and the crew dashboard artifact.
- confirmed_by_owner: yes (2026-10-03, all subtasks pre-approved)
- milestone: M1
- next_step: none
- result: Acceptance passed: repeatable command and launcher (sys-stress), baseline and final tables (stress-test-m1, stress-final-m1), ceiling about 500–600 robots, M1 cap of 100 safe with margin, ranked leads in perf-risks; no optimization coded.

## Subtasks
1. task-013-01-baseline — cpp: stress spawn command, -game CSV profile, S0 at 50 to 400 chasers, report
2. task-013-02-final — cpp: same after task-016 with S0, S2, S7, compare, leads

## Relations
- [[tasks-index]] — parent index.
- [[task-013-01-baseline]] — step 1, cpp.
- [[task-013-02-final]] — step 2, cpp.
