---
id: task-435-remove-robot-hit-flash
description: "Global task: A robot that takes a hit no longer flashes: the feature of task-433 is removed for good."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Remove the robot hit flash

- request: Owner 2026-11-05 (verbatim, whole list): "Voici une liste de tâche, découpe les avec le planneur, pose les questions et puis lance l'agent executor par lot de tâche ou par tâche selon ce que tu penses qu'il sera le mieux : * Retire le flash sur le robot quand ils sont touchés. * Améliorer la particule de mort du robot * La particule d'impact de tir doit s'activer aussi quand tu touches un mur. * Il faut aussi une particule de débris de terre lorsqu'on creuse. * Lors d'une explosion, si des murs sont détruit, les débris doivent aller dans la direction opposée d'ou vient l'explosion * Il faut retravailler le muzzle flash * Il faut retravailler l'effet de level up ( un text qui pop comme les dégâts, mais pas d'effet d'arc et en couleur dorée ) + particule au sol + ennemi légèrement repoussé ( la particule doit donner cet effet ) * La particule de sous les hélices de l'extracteur font plus pensé à de la pluie qu'autre chose, il faut modifié ça * Il faudrait aussi représenter le souffle des hélices de l'extracteur au sol ( qui diminue en fonction de sa hauteur ( et elle disparait quand il est assez haut ( + de 4m ) * Les hélices projectent de l'ombre de l'extracteur, retire ça * il faudrait que la light du joueur projette de l'ombre des murs ( seulement )" This task is the point "Retire le flash sur le robot quand ils sont touchés."
- goal: A robot that takes a hit keeps its colours; the flash of task-433 no longer exists in the project.
- acceptance: A1 in game, no robot class changes colour or brightness when it takes damage from any source; A2 the chaser wind-up telegraph, the kamikaze core pulse, baked animation, damage, death effect and impact sparks are unchanged; A3 no trace of the flash stays in code, materials, instances, tests or docs apart from history; A4 existing tests pass, the flash test goes with the feature; A5 No capture and no stress run in this task unless the owner asks: he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it.
- gdd_check: Read readability-rules, enemy-chaser, enemy-death, bp-robot-hit-flash, task-433. Conflict, by the owner's own new word: the hit flash rule of readability-rules and "hits are a flash" of enemy-chaser, both edited at confirmation. The wind-up "stops and flashes" telegraph of enemy-chaser is a different rule and stays (rules versus rendering). In scope: M3 juice, submission November 9.
- confirmed_by_owner: no (plan and questions handed back 2026-11-05)
- milestone: M2
- next_step: task-435-01-remove-trigger
- started: open
- waiting_for: owner answers to the questions of the 2026-11-05 FX and light list (Q1 to Q17, "ok" accepts every default)
- result: open

## Questions
Asked 2026-11-05, each with a default; open until the owner answers.
- Q1 « Retire le flash » : on le supprime pour de bon ? Default: oui, tout ce que task-433 a ajouté part (code, fonction de matériau, test, doc), git garde l'historique pour le rétablir. Le clignotement du chaser avant de frapper reste, c'est un autre signal. L'autre choix : le désactiver et garder le code.
- Q2 Pour les onze tâches de cette liste : captures et mesure de coût ? Default: aucune capture et aucun stress test, tu juges en jeu comme pour le lot 1, chaque rapport te liste quoi regarder. Coût borné par construction (pool, particules CPU, comptes limités). Vaut aussi pour l'ombre de la lumière du joueur, voir Q17.

## Subtasks
1. task-435-01-remove-trigger (vault/reference/tasks/task-435-remove-robot-hit-flash/task-435-01-remove-trigger.md) — cpp: Taking damage no longer raises any flash value on a robot, and the flash test is gone.
2. task-435-02-remove-materials (vault/reference/tasks/task-435-remove-robot-hit-flash/task-435-02-remove-materials.md) — bp: The robot materials and instances no longer read the flash and the flash function is deleted.
3. task-435-03-doc (vault/reference/tasks/task-435-remove-robot-hit-flash/task-435-03-doc.md) — doc: Remove the flash from the docs that describe it.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-435-01-remove-trigger]] — step 1, cpp.
- [[task-435-02-remove-materials]] — step 2, bp.
- [[task-435-03-doc]] — step 3, doc.
