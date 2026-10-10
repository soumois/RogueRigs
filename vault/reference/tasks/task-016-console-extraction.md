---
id: task-016-console-extraction
description: "Global task: minimal alert bar, command console with marked rocks and drone hack, shuttle landing with countdown, success and run-lost screens."
parent: tasks-done-index
kind: task
status: done
---
# Console hack and shuttle extraction

- request: "Concernant la task 12, je valide. J'aurais juste d'autre points, les kamikaze doit être moins nombreux que les chasseurs. Aussi, tu peux multiplier par 5, le nombre d'enemi. Mais essaye qu'ils arrivent plus en "paquet" qu'au compte goute. [...] Tu peux commencer à faire l'amélioration des upgrades, ne fait que les caractéristiques de base ( pas le nombre de balle ) et fait déjà le système de rareté. [...] Lorsque je level up , il faut un effet, une récupération de pv ( un son plus tard ) et seulement après le jeu se met en pause pour proposer l'amélioration. Pour l'instant, on aura le droit à un bouton pour refaire une sélection des propositions ( mais qu'une fois ) [...] tu peux enchainer sur le fusil à pompe,le piratage de la console et extraction de navette. Et lance un stress test toi même pour commencer [...]" Then: "rajoute le fire rate, la vitesse de rechargement, les dégats pour les améliorations"; plan confirmed "oui go" (2026-10-03).
- goal: the alert bar fills; at 40 % a console drops with marked rocks; the drone hacks it while the miner stays in its zone; when the bar is full a shuttle lands far away with a 45 s countdown; reaching it wins, missing it loses; both restart the map.
- acceptance: (1) build exits 0; all tests pass, new ones included; (2) alert bar fill, kill bonus, pause while the console is pending (tests); (3) console drop distance, marked rocks must be destroyed, hack only in zone, drain, completion (tests); (4) shuttle distance, countdown, win and loss (tests); (5) PIE loop played end to end; (6) docs [[sys-difficulty]], new system docs, Blueprint docs; lint clean; committed.
- gdd_check: Read [[level-flow]], [[difficulty]], [[objective-console]], [[extraction]], [[drone]], [[destruction-rules]], [[color-code]]. Alert gains, console and shuttle values open → owner set provisional defaults. Owner decisions of 2026-10-03 written into the GDD before planning (level-up heal vs heal-ore-only, rarities moved to M1, reroll, spawn packs, all open values set as provisional defaults). TaskCreate and TaskUpdate are not available in this session: progress is tracked in these nodes and the crew dashboard artifact.
- confirmed_by_owner: yes (2026-10-03, all subtasks pre-approved)
- milestone: M1
- next_step: none
- result: All acceptance checks passed: 95/95 headless tests (alert time and kills, pause on pending console, console placement, marked rocks, hack rules and drain, shuttle distance, countdown, win, loss, real pause on the end screen, restart); PIE loop played end to end through the cheat property route; docs sys-objectives (new), sys-difficulty, sys-drone, sys-hud, sys-cheats, Blueprint docs. Owner to review: console prefers cells with rocks to mark; shuttle board radius 1 cell; console gauge is a glow, not a bar; visuals not seen on screen. About 880k tokens over four subtasks.

## Subtasks
1. task-016-01-alert-console — cpp: run flow, alert bar, console drop, marked rocks, drone hack, tests
2. task-016-02-shuttle — cpp: shuttle landing, countdown, win and loss, restart, tests
3. task-016-03-bp-objectives — bp: console, markers, gauges, shuttle, alert HUD, end screens, PIE
4. task-016-04-tests-integration — cpp: full loop integration test with final Blueprints

## Relations
- [[tasks-done-index]] — parent index.
- [[task-016-01-alert-console]] — step 1, cpp.
- [[task-016-02-shuttle]] — step 2, cpp.
- [[task-016-03-bp-objectives]] — step 3, bp.
- [[task-016-04-tests-integration]] — step 4, cpp.
