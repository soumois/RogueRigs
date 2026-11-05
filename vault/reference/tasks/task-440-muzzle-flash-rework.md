---
id: task-440-muzzle-flash-rework
description: "Global task: A new look for the miner weapon muzzle flash: forward star, soft halo, white-gold."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Muzzle flash rework

- request: Owner 2026-11-05 (verbatim, whole list): "Voici une liste de tâche, découpe les avec le planneur, pose les questions et puis lance l'agent executor par lot de tâche ou par tâche selon ce que tu penses qu'il sera le mieux : * Retire le flash sur le robot quand ils sont touchés. * Améliorer la particule de mort du robot * La particule d'impact de tir doit s'activer aussi quand tu touches un mur. * Il faut aussi une particule de débris de terre lorsqu'on creuse. * Lors d'une explosion, si des murs sont détruit, les débris doivent aller dans la direction opposée d'ou vient l'explosion * Il faut retravailler le muzzle flash * Il faut retravailler l'effet de level up ( un text qui pop comme les dégâts, mais pas d'effet d'arc et en couleur dorée ) + particule au sol + ennemi légèrement repoussé ( la particule doit donner cet effet ) * La particule de sous les hélices de l'extracteur font plus pensé à de la pluie qu'autre chose, il faut modifié ça * Il faudrait aussi représenter le souffle des hélices de l'extracteur au sol ( qui diminue en fonction de sa hauteur ( et elle disparait quand il est assez haut ( + de 4m ) * Les hélices projectent de l'ombre de l'extracteur, retire ça * il faudrait que la light du joueur projette de l'ombre des murs ( seulement )" This task is the point "Il faut retravailler le muzzle flash".
- goal: The muzzle flash reads as a real, punchy flash at the game camera distance.
- acceptance: A1 in game, each rifle bullet and shotgun volley shows a white-gold star of 3 to 4 forward flames and a short soft halo, on a dedicated soft material rather than the square default sprite, at most 0.08 s; A2 visibly larger than today and larger on the shotgun; A3 no light, no red, no orange; A4 same spawn rule: one per rifle bullet, one per shotgun volley, turrets and robots none; A5 cost per shot no higher than today in particle count order, at most 10 sprites; A6 shot behaviour unchanged, Weapons, Shotgun and Turret test families pass; A7 No capture and no stress run in this task unless the owner asks: he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it.
- gdd_check: Read art-style-guide A8, color-code, bp-muzzle-flash. "Retravailler" states no direction: Q9. No conflict. Unverified since task-414: look on screen. A8 edited at confirmation.
- confirmed_by_owner: no (plan and questions handed back 2026-11-05)
- milestone: M2
- next_step: task-440-01-flash-system
- started: open
- waiting_for: owner answers to the questions of the 2026-11-05 FX and light list (Q1 to Q17, "ok" accepts every default)
- result: open

## Questions
Asked 2026-11-05, each with a default; open until the owner answers.
- Q9 Retravailler dans quel sens ? Default: une étoile blanc-or de 3 à 4 flammes vers l'avant et un halo doux très court, nettement plus grande qu'aujourd'hui, au plus 0,08 s et 10 particules, sans lumière dynamique, plus grande sur le fusil à pompe que sur le fusil. Si tu as une référence ou un défaut précis, dis-le.

## Subtasks
1. task-440-01-flash-system (vault/reference/tasks/task-440-muzzle-flash-rework/task-440-01-flash-system.md) — bp: The muzzle flash system and its soft material have the new look, larger on the shotgun.
2. task-440-02-doc (vault/reference/tasks/task-440-muzzle-flash-rework/task-440-02-doc.md) — doc: Document the new values.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-440-01-flash-system]] — step 1, bp.
- [[task-440-02-doc]] — step 2, doc.
