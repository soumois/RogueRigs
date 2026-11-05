---
id: task-436-robot-death-particle
description: "Global task: A cartoon robot death flash after the owner reference instead of a plain smoke cloud, bounded in a horde."
parent: tasks-done-index
kind: task
status: done
---
# Better robot death particle

- request: Owner 2026-11-05 (verbatim, whole list): "Voici une liste de tâche, découpe les avec le planneur, pose les questions et puis lance l'agent executor par lot de tâche ou par tâche selon ce que tu penses qu'il sera le mieux : * Retire le flash sur le robot quand ils sont touchés. * Améliorer la particule de mort du robot * La particule d'impact de tir doit s'activer aussi quand tu touches un mur. * Il faut aussi une particule de débris de terre lorsqu'on creuse. * Lors d'une explosion, si des murs sont détruit, les débris doivent aller dans la direction opposée d'ou vient l'explosion * Il faut retravailler le muzzle flash * Il faut retravailler l'effet de level up ( un text qui pop comme les dégâts, mais pas d'effet d'arc et en couleur dorée ) + particule au sol + ennemi légèrement repoussé ( la particule doit donner cet effet ) * La particule de sous les hélices de l'extracteur font plus pensé à de la pluie qu'autre chose, il faut modifié ça * Il faudrait aussi représenter le souffle des hélices de l'extracteur au sol ( qui diminue en fonction de sa hauteur ( et elle disparait quand il est assez haut ( + de 4m ) * Les hélices projectent de l'ombre de l'extracteur, retire ça * il faudrait que la light du joueur projette de l'ombre des murs ( seulement )" This task is the point "Améliorer la particule de mort du robot". Owner answers 2026-11-05 (relayed by the main session, verbatim): "Q2 - Remesurer les tokens, je n'ai aucune idée du couts pour les tâches 432/433 et 434 par exemple. Q3 - C'est un bete nuage de fumée, j'aimerais des particules un peu plus cartoon pour que cela flash ( voir image 1 comme exemple ). Q5 - Les murs que l'on peut creuser. Tu avais marqué dans ton rapport de la précedante tâche qu'il n'y avait pas d'éteincelles lorsqu'on tire sur la roche ou du minerai. Je veux changer ça. Q9 - Il est trop basique, tu peux voir dans la vidéo ( tu peux voir de 13:28 et 13:33 ). Q14 : l'extractor entier ne doit pas projeter d'ombre ( tu peux voir sur l'image 2 qu'on voit l'ombre de deux hélices ). Q16 j 'ai déjà bougé moi même la hauteur de la lumiére. Q17 : Non, mais je veux bien les couts en tokens" Owner answer 2026-11-05 (relayed by the main session, verbatim): "Q18 : Il ne faut être si rigide sur le code couleur, une explosion dans l'esprit collective a aussi du rouge. Le code couleur ne peut pas non plus aller à l'encontre de ce qu'attend un joueur de l'explosion"
- goal: A robot death flashes in a cartoon way instead of reading as a plain smoke cloud, still hides the shrink, and stays bounded at horde scale.
- acceptance: A1 in game, a robot death reads as the cartoon flash of the owner's reference Art/Source/robot-death-fx/20261105-owner-ref-cartoon-explosion.png (a reference for shape and style, not an asset to ship): small flash, white and yellow core growing with jagged cel-shaded edges in flat colour bands, chunky blobs and thin straight lines thrown outward, then burnt puffs; never a soft smoke cloud; A2 colours as in the reference: white and yellow core, orange and red-orange outer bands, dark red end puffs (Q18, owner: red allowed); A3 the 0.15 s shrink stays hidden, the thrown pieces and XP drop unchanged; A4 at most 20 CPU sprites per death and about 0.4 s, the existing pool bound kept; A5 the heavy death has the same look about 3 times larger; A6 kamikaze death unchanged; A7 No capture and no stress run in this task unless the owner asks: he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it. The report also gives the token cost of this task, per subtask and in total, from cost-log (units, turns, runs, resumes): owner Q2 and Q17, 2026-11-05.
- gdd_check: Read enemy-death, color-code, readability-rules, bp-robot-death-puff. "Améliorer" states no direction: Q3, answered with reference Art/Source/robot-death-fx/20261105-owner-ref-cartoon-explosion.png. Conflict: the sheet has red-orange outer bands and dark red puffs, against readability-rules (red = enemy fire or healing): Q18, the owner refused the no-red default; readability-rules gets his explosion rule at confirmation. Q4 not answered, default kept. enemy-death puff line edited at confirmation. Values provisional.
- confirmed_by_owner: yes (2026-11-05, "Q18 : Il ne faut être si rigide sur le code couleur, une explosion dans l'esprit collective a aussi du rouge. Le code couleur ne peut pas non plus aller à l'encontre de ce qu'attend un joueur de l'explosion")
- milestone: M2
- next_step: none
- started: 2026-11-05T17:37:13Z
- waiting_for: none
- result: Done. bp-robot-death-puff and bp-heavy-death-puff state the new values; bp-kamikaze-death-puff created, bp-kamikaze and blueprints-fx-index updated (56596513); lint clean.

## Questions
Decided 2026-11-05 (owner, relayed by the main session): Q3 and Q18 by his words written in them, Q4 by its default, kept when he left it unanswered; values provisional, owner 2026-11-05.
- Q3 Améliorer dans quel sens ? Default (selon la réponse du propriétaire, 2026-11-05 : « Q3 - C'est un bete nuage de fumée, j'aimerais des particules un peu plus cartoon pour que cela flash ( voir image 1 comme exemple ) ») : le flash cartoon de Art/Source/robot-death-fx/20261105-owner-ref-cartoon-explosion.png, en au plus 20 particules et environ 0,4 s.
- Q4 Le heavy aussi ? Default: oui, même look environ 3 fois plus grand, comme son puff actuel.
- Q18 Ton image a des bords rouge-orangé et des bouffées rouge sombre, or le rouge est réservé aux tirs ennemis et au soin, et l'orange veut dire « explose ». Default proposé sans rouge, refusé. Décision du propriétaire, 2026-11-05 : « Q18 : Il ne faut être si rigide sur le code couleur, une explosion dans l'esprit collective a aussi du rouge. Le code couleur ne peut pas non plus aller à l'encontre de ce qu'attend un joueur de l'explosion » : les couleurs de l'image, rouge compris.

## Subtasks
1. task-436-01-robot-puff (vault/reference/tasks/task-436-robot-death-particle/task-436-01-robot-puff.md) — bp: Representative slice: the robot death puff has its new look.
2. task-436-02-heavy-puff (vault/reference/tasks/task-436-robot-death-particle/task-436-02-heavy-puff.md) — bp: The heavy death puff has the same look about 3 times larger.
3. task-436-03-doc (vault/reference/tasks/task-436-robot-death-particle/task-436-03-doc.md) — doc: Document the new values of both puffs.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-436-01-robot-puff]] — step 1, bp.
- [[task-436-02-heavy-puff]] — step 2, bp.
- [[task-436-03-doc]] — step 3, doc.
