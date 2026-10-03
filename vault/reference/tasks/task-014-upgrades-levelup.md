---
id: task-014-upgrades-levelup
description: "Global task: 100 robots in packs with fewer kamikazes, stat upgrades with rarities, level-up heal, effect, pause, 3 cards and one reroll."
parent: tasks-index
kind: task
status: done
---
# Spawn packs, upgrades and level-up

- request: "Concernant la task 12, je valide. J'aurais juste d'autre points, les kamikaze doit être moins nombreux que les chasseurs. Aussi, tu peux multiplier par 5, le nombre d'enemi. Mais essaye qu'ils arrivent plus en "paquet" qu'au compte goute. [...] Tu peux commencer à faire l'amélioration des upgrades, ne fait que les caractéristiques de base ( pas le nombre de balle ) et fait déjà le système de rareté. [...] Lorsque je level up , il faut un effet, une récupération de pv ( un son plus tard ) et seulement après le jeu se met en pause pour proposer l'amélioration. Pour l'instant, on aura le droit à un bouton pour refaire une sélection des propositions ( mais qu'une fois ) [...] tu peux enchainer sur le fusil à pompe,le piratage de la console et extraction de navette. Et lance un stress test toi même pour commencer [...]" Then: "rajoute le fire rate, la vitesse de rechargement, les dégats pour les améliorations"; plan confirmed "oui go" (2026-10-03).
- goal: robots come in packs (100 max, 15 % kamikazes); each level-up shows a ring, heals 20 %, then pauses on 3 rarity-colored upgrade cards (player and owned-weapon stats) with one reroll; the picked upgrade changes the stat.
- acceptance: (1) build exits 0; all `RogueRigs.*` tests pass headless, new ones included; (2) spawner: 30 s trickle then packs of 8 to 15 from one direction, cap 100, kamikaze share 15 % (tests); (3) upgrades: CSV value table matches [[upgrades]], rarity draw matches [[rarities]], no card targets an unowned weapon, no magazine card, reroll once (tests); (4) level-up: heal 20 %, pause after 0.8 s, chained screens, pick resumes (tests); (5) docs [[sys-spawner]], [[sys-upgrades]], [[sys-progression]], [[sys-hud]], Blueprint docs; lint clean; every subtask committed.
- gdd_check: Read [[enemies-index]], [[enemy-kamikaze]], [[upgrades]], [[rarities]], [[player-stats]], [[xp-level-up]], [[burst-rifle]]. Owner decisions of 2026-10-03 written into the GDD before planning (level-up heal vs heal-ore-only, rarities moved to M1, reroll, spawn packs, all open values set as provisional defaults). TaskCreate and TaskUpdate are not available in this session: progress is tracked in these nodes and the crew dashboard artifact.
- confirmed_by_owner: yes (2026-10-03, all subtasks pre-approved)
- milestone: M1
- next_step: none
- result: All acceptance checks passed: 76/76 headless tests; spawner packs, cap 100, kamikaze 15 %; upgrades with rarities, owned-weapon targets only, no magazine, reroll once; level-up heal 20 %, pause after 0.8 s, chained screens; card UI and level-up disc verified in PIE and by a real-pause integration test. Docs sys-spawner, sys-upgrades, sys-level-up (new), sys-progression, sys-hud, sys-cheats (new), Blueprint docs. Open for the owner: card widths differ at 1920 px (cosmetic); the level-up ring is a filled disc; mouse clicks on cards not confirmed in PIE (keyboard Enter was). About 1M tokens over five subtasks.

## Subtasks
1. task-014-01-spawn-packs — cpp: pack spawning, cap 100, kamikaze 15 %, tests
2. task-014-02-upgrade-system — cpp: value table, rarity draw, offer, apply, reroll, tests
3. task-014-03-levelup-flow — cpp: heal, effect event, delayed pause, chained choices, tests
4. task-014-04-bp-upgrade-ui — bp: level-up ring, card widget with reroll, spawner values, PIE
5. task-014-05-tests-integration — cpp: kill, level up, pick, stat changed, with final Blueprints

## Relations
- [[tasks-index]] — parent index.
- [[task-014-01-spawn-packs]] — step 1, cpp.
- [[task-014-02-upgrade-system]] — step 2, cpp.
- [[task-014-03-levelup-flow]] — step 3, cpp.
- [[task-014-04-bp-upgrade-ui]] — step 4, bp.
- [[task-014-05-tests-integration]] — step 5, cpp.
