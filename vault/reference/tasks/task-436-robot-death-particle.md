---
id: task-436-robot-death-particle
description: "Global task: A stronger robot death puff that stays readable in a horde and no longer uses the orange of explosions."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Better robot death particle

- request: Owner 2026-11-05 (verbatim, whole list): "Voici une liste de tâche, découpe les avec le planneur, pose les questions et puis lance l'agent executor par lot de tâche ou par tâche selon ce que tu penses qu'il sera le mieux : * Retire le flash sur le robot quand ils sont touchés. * Améliorer la particule de mort du robot * La particule d'impact de tir doit s'activer aussi quand tu touches un mur. * Il faut aussi une particule de débris de terre lorsqu'on creuse. * Lors d'une explosion, si des murs sont détruit, les débris doivent aller dans la direction opposée d'ou vient l'explosion * Il faut retravailler le muzzle flash * Il faut retravailler l'effet de level up ( un text qui pop comme les dégâts, mais pas d'effet d'arc et en couleur dorée ) + particule au sol + ennemi légèrement repoussé ( la particule doit donner cet effet ) * La particule de sous les hélices de l'extracteur font plus pensé à de la pluie qu'autre chose, il faut modifié ça * Il faudrait aussi représenter le souffle des hélices de l'extracteur au sol ( qui diminue en fonction de sa hauteur ( et elle disparait quand il est assez haut ( + de 4m ) * Les hélices projectent de l'ombre de l'extracteur, retire ça * il faudrait que la light du joueur projette de l'ombre des murs ( seulement )" This task is the point "Améliorer la particule de mort du robot".
- goal: A robot death is more satisfying to see, still hides the shrink, and stays bounded at horde scale.
- acceptance: A1 in game, a robot death shows a very short white flash, 6 to 8 fast white-gold sparks and a dark grey smoke of about 0.4 s, on a soft round sprite rather than a square default sprite; A2 no orange in it, no red; A3 the 0.15 s shrink stays hidden, the thrown pieces and XP drop unchanged; A4 at most 20 CPU sprites per death, the existing pool bound kept; A5 the heavy death has the same look about 3 times larger; A6 kamikaze death unchanged; A7 No capture and no stress run in this task unless the owner asks: he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it.
- gdd_check: Read enemy-death, color-code, readability-rules, bp-robot-death-puff. "Améliorer" states no direction: Q3. Tension already flagged: the warm orange puff against "orange means explodes" in a horde; the default ends it. enemy-death puff line edited at confirmation. Values provisional.
- confirmed_by_owner: no (plan and questions handed back 2026-11-05)
- milestone: M2
- next_step: task-436-01-robot-puff
- started: open
- waiting_for: owner answers to the questions of the 2026-11-05 FX and light list (Q1 to Q17, "ok" accepts every default)
- result: open

## Questions
Asked 2026-11-05, each with a default; open until the owner answers.
- Q3 Améliorer dans quel sens ? Default: petite destruction mécanique lisible en horde : flash blanc très court, 6 à 8 étincelles blanc-or qui partent vite, fumée gris sombre d'environ 0,4 s, au plus 20 particules. Plus d'orange, réservé à « explose » (kamikaze, baril), ce qui tranche la question que je t'avais posée. L'autre choix : garder l'orange actuel avec plus de volume. Si tu as une référence précise, dis-la.
- Q4 Le heavy aussi ? Default: oui, même look environ 3 fois plus grand, comme son puff actuel.

## Subtasks
1. task-436-01-robot-puff (vault/reference/tasks/task-436-robot-death-particle/task-436-01-robot-puff.md) — bp: Representative slice: the robot death puff has its new look.
2. task-436-02-heavy-puff (vault/reference/tasks/task-436-robot-death-particle/task-436-02-heavy-puff.md) — bp: The heavy death puff has the same look about 3 times larger.
3. task-436-03-doc (vault/reference/tasks/task-436-robot-death-particle/task-436-03-doc.md) — doc: Document the new values of both puffs.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-436-01-robot-puff]] — step 1, bp.
- [[task-436-02-heavy-puff]] — step 2, bp.
- [[task-436-03-doc]] — step 3, doc.
