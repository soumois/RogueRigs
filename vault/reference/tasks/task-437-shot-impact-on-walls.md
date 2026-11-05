---
id: task-437-shot-impact-on-walls
description: "Global task: The player shot impact sparks also play when the shot is stopped by a wall."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Shot impact sparks on walls

- request: Owner 2026-11-05 (verbatim, whole list): "Voici une liste de tâche, découpe les avec le planneur, pose les questions et puis lance l'agent executor par lot de tâche ou par tâche selon ce que tu penses qu'il sera le mieux : * Retire le flash sur le robot quand ils sont touchés. * Améliorer la particule de mort du robot * La particule d'impact de tir doit s'activer aussi quand tu touches un mur. * Il faut aussi une particule de débris de terre lorsqu'on creuse. * Lors d'une explosion, si des murs sont détruit, les débris doivent aller dans la direction opposée d'ou vient l'explosion * Il faut retravailler le muzzle flash * Il faut retravailler l'effet de level up ( un text qui pop comme les dégâts, mais pas d'effet d'arc et en couleur dorée ) + particule au sol + ennemi légèrement repoussé ( la particule doit donner cet effet ) * La particule de sous les hélices de l'extracteur font plus pensé à de la pluie qu'autre chose, il faut modifié ça * Il faudrait aussi représenter le souffle des hélices de l'extracteur au sol ( qui diminue en fonction de sa hauteur ( et elle disparait quand il est assez haut ( + de 4m ) * Les hélices projectent de l'ombre de l'extracteur, retire ça * il faudrait que la light du joueur projette de l'ombre des murs ( seulement )" This task is the point "La particule d'impact de tir doit s'activer aussi quand tu touches un mur."
- goal: A miner or turret shot stopped by a block shows the same impact sparks as on a robot.
- acceptance: A1 in game, a miner or turret shot stopped by rock, ore or heal ore shows the white-gold impact sparks at the hit point; A2 robots and barrels keep theirs, robot shots show none, the drone laser unchanged; A3 same system and same pool, a full pool skips the spark; A4 shots never break blocks, damage and piercing unchanged, existing tests pass, only expectations this change breaks are updated; A5 No capture and no stress run in this task unless the owner asks: he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it.
- gdd_check: Read task-432, art-style-guide A8, destruction-rules, bp-shot-impact-spark. Reverses the default the owner accepted on task-432 Q1 ("none on rock or ore"): A8 edited at confirmation. Known risk he had accepted avoiding: sparks on rock may resemble the drone laser sparks; he judges in game.
- confirmed_by_owner: no (plan and questions handed back 2026-11-05)
- milestone: M2
- next_step: task-437-01-spark-on-block
- started: open
- waiting_for: owner answers to the questions of the 2026-11-05 FX and light list (Q1 to Q17, "ok" accepts every default)
- result: open

## Questions
Asked 2026-11-05, each with a default; open until the owner answers.
- Q5 Quels murs, et quelle particule ? Default: tous les blocs qui arrêtent un tir (roche, minerai, minerai de soin), avec exactement la même particule blanc-or que sur les robots. Cela remplace ton choix du lot 1 ; ça pourra un peu ressembler aux étincelles du laser de forage, tu juges en jeu.

## Subtasks
1. task-437-01-spark-on-block (vault/reference/tasks/task-437-shot-impact-on-walls/task-437-01-spark-on-block.md) — cpp: A qualifying shot stopped by a block requests the same impact spark at the hit point.
2. task-437-02-doc (vault/reference/tasks/task-437-shot-impact-on-walls/task-437-02-doc.md) — doc: Document the new targets of the spark.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-437-01-spark-on-block]] — step 1, cpp.
- [[task-437-02-doc]] — step 2, doc.
