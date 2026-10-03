---
id: task-015-shotgun
description: "Global task: shotgun conical wave with line of sight, crit per enemy, knockback; given at start with the rifle; its upgrades join the pool."
parent: tasks-index
kind: task
status: done
---
# Shotgun

- request: "Concernant la task 12, je valide. J'aurais juste d'autre points, les kamikaze doit être moins nombreux que les chasseurs. Aussi, tu peux multiplier par 5, le nombre d'enemi. Mais essaye qu'ils arrivent plus en "paquet" qu'au compte goute. [...] Tu peux commencer à faire l'amélioration des upgrades, ne fait que les caractéristiques de base ( pas le nombre de balle ) et fait déjà le système de rareté. [...] Lorsque je level up , il faut un effet, une récupération de pv ( un son plus tard ) et seulement après le jeu se met en pause pour proposer l'amélioration. Pour l'instant, on aura le droit à un bouton pour refaire une sélection des propositions ( mais qu'une fois ) [...] tu peux enchainer sur le fusil à pompe,le piratage de la console et extraction de navette. Et lance un stress test toi même pour commencer [...]" Then: "rajoute le fire rate, la vitesse de rechargement, les dégats pour les améliorations"; plan confirmed "oui go" (2026-10-03).
- goal: the miner starts with both weapons; the shotgun auto-fires a 60° wave that hits every robot in range with line of sight, crits per robot and pushes them back; shotgun-targeted upgrade cards appear.
- acceptance: (1) build exits 0; all tests pass, new ones included; (2) cone, range, line of sight, one crit roll per enemy, knockback reduced by resistance (tests); (3) shotgun upgrades offered only when owned (test); (4) white-gold wave visible and cut by rock in PIE; (5) docs [[sys-weapons]], new Blueprint doc; lint clean; committed.
- gdd_check: Read [[shotgun]], [[burst-rifle]], [[upgrades]], [[miner]], [[color-code]]. Weapon acquisition open → owner: both weapons at start in M1. Owner decisions of 2026-10-03 written into the GDD before planning (level-up heal vs heal-ore-only, rarities moved to M1, reroll, spawn packs, all open values set as provisional defaults). TaskCreate and TaskUpdate are not available in this session: progress is tracked in these nodes and the crew dashboard artifact.
- confirmed_by_owner: yes (2026-10-03, all subtasks pre-approved)
- milestone: M1
- next_step: none
- result: Acceptance 1 to 3 and 5 passed: 82/82 tests (cone, range, line of sight, independent crits, knockback × resistance, shotgun cards only when equipped); both weapons at start; docs. Acceptance 4 partly: the wave fires and hits in PIE, its rays use the rock-clipped distance, but the flash and the rock cut were not seen on screen (no screenshot route). For the owner to judge: knockback strength (friction stops robots fast), flash look.

## Subtasks
1. task-015-01-shotgun-cpp — cpp: conical wave weapon, LOS, crit per enemy, knockback, start loadout, tests
2. task-015-02-bp-shotgun — bp: wave VFX, weapon Blueprint, loadout, PIE

## Relations
- [[tasks-index]] — parent index.
- [[task-015-01-shotgun-cpp]] — step 1, cpp.
- [[task-015-02-bp-shotgun]] — step 2, bp.
