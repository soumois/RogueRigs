---
id: task-440-muzzle-flash-rework
description: "Global task: A stylised, varied white-gold muzzle flash for the miner weapons, after the owner reference video."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Muzzle flash rework

- request: Owner 2026-11-05 (verbatim, whole list): "Voici une liste de tâche, découpe les avec le planneur, pose les questions et puis lance l'agent executor par lot de tâche ou par tâche selon ce que tu penses qu'il sera le mieux : * Retire le flash sur le robot quand ils sont touchés. * Améliorer la particule de mort du robot * La particule d'impact de tir doit s'activer aussi quand tu touches un mur. * Il faut aussi une particule de débris de terre lorsqu'on creuse. * Lors d'une explosion, si des murs sont détruit, les débris doivent aller dans la direction opposée d'ou vient l'explosion * Il faut retravailler le muzzle flash * Il faut retravailler l'effet de level up ( un text qui pop comme les dégâts, mais pas d'effet d'arc et en couleur dorée ) + particule au sol + ennemi légèrement repoussé ( la particule doit donner cet effet ) * La particule de sous les hélices de l'extracteur font plus pensé à de la pluie qu'autre chose, il faut modifié ça * Il faudrait aussi représenter le souffle des hélices de l'extracteur au sol ( qui diminue en fonction de sa hauteur ( et elle disparait quand il est assez haut ( + de 4m ) * Les hélices projectent de l'ombre de l'extracteur, retire ça * il faudrait que la light du joueur projette de l'ombre des murs ( seulement )" This task is the point "Il faut retravailler le muzzle flash". Owner answers 2026-11-05 (relayed by the main session, verbatim): "Q2 - Remesurer les tokens, je n'ai aucune idée du couts pour les tâches 432/433 et 434 par exemple. Q3 - C'est un bete nuage de fumée, j'aimerais des particules un peu plus cartoon pour que cela flash ( voir image 1 comme exemple ). Q5 - Les murs que l'on peut creuser. Tu avais marqué dans ton rapport de la précedante tâche qu'il n'y avait pas d'éteincelles lorsqu'on tire sur la roche ou du minerai. Je veux changer ça. Q9 - Il est trop basique, tu peux voir dans la vidéo ( tu peux voir de 13:28 et 13:33 ). Q14 : l'extractor entier ne doit pas projeter d'ombre ( tu peux voir sur l'image 2 qu'on voit l'ombre de deux hélices ). Q16 j 'ai déjà bougé moi même la hauteur de la lumiére. Q17 : Non, mais je veux bien les couts en tokens". Owner answer 2026-11-05 (relayed by the main session, verbatim): "oui (réponse à : « Q19 : tu valides la version par défaut, c'est-à-dire les flashs chauds de la vidéo en blanc-or, ou tu préfères un flash précis, avec sa position dans la vidéo ? »)" = the plan and the defaults of Q9, Q19 accepted.
- goal: The muzzle flash stops looking basic: stylised and varied like the owner's reference, readable at the game camera distance.
- acceptance: A1 in game, each rifle bullet and shotgun volley shows a stylised white-gold flash after the warm flashes of Art/Source/muzzle-flash/20261105-owner-ref-video-1328-1333-sheet.png (video of the owner, 13:28 to 13:33; a shape reference, not an asset to ship): crisp thin shapes that change from shot to shot (forward flame, curved side wings, crossed blades, star), a bright core and a long thin forward streak, at most 0.1 s, on a dedicated material rather than the square default sprite; A2 visibly larger than today and larger on the shotgun; A3 no light, no red, none of the blue, green or pink variants of the video; A4 same spawn rule: one per rifle bullet, one per shotgun volley, turrets and robots none; A5 cost per shot no higher than today in particle count order, at most 10 sprites; A6 shot behaviour unchanged, Weapons, Shotgun and Turret test families pass; A7 No capture and no stress run in this task unless the owner asks: he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it. The report also gives the token cost of this task, per subtask and in total, from cost-log (units, turns, runs, resumes): owner Q2 and Q17, 2026-11-05.
- gdd_check: Read art-style-guide A8, color-code, bp-muzzle-flash. "Retravailler" states no direction: Q9, answered with the video (frames in Art/Source/muzzle-flash/20261105-owner-ref-video-1328-1333-sheet.png, viewed by the Planner). It shows about eight flashes of several colours looping on a board, so which ones he means is Q19. The colour rule (player fire is white-gold, A8) excludes copying the colours. No conflict. Unverified since task-414: look on screen. A8 edited at confirmation.
- confirmed_by_owner: yes (2026-11-05, "oui (réponse à : « Q19 : tu valides la version par défaut, c'est-à-dire les flashs chauds de la vidéo en blanc-or, ou tu préfères un flash précis, avec sa position dans la vidéo ? »)")
- milestone: M2
- next_step: task-440-02-doc
- started: 2026-11-05T19:39:46Z
- waiting_for: none
- result: open

## Questions
Decided 2026-11-05 (owner "oui (réponse à : « Q19 : tu valides la version par défaut, c'est-à-dire les flashs chauds de la vidéo en blanc-or, ou tu préfères un flash précis, avec sa position dans la vidéo ? »)"): every default below is accepted; values provisional, owner 2026-11-05.
- Q9 Retravailler dans quel sens ? Default (selon la réponse du propriétaire, 2026-11-05 : « Q9 - Il est trop basique, tu peux voir dans la vidéo ( tu peux voir de 13:28 et 13:33 ) ») : un flash stylisé d'après la vidéo, images dans Art/Source/muzzle-flash/20261105-owner-ref-video-1328-1333-sheet.png.
- Q19 La vidéo montre environ huit flashs différents. Default: je reprends les flashs chauds : formes fines et nettes qui changent à chaque tir (flamme vers l'avant, ailes courbes sur les côtés, lames croisées, étoile), noyau brillant et longue traînée fine vers l'avant, tout en blanc-or comme les tirs du joueur, au plus 0,1 s et 10 particules, sans lumière dynamique, plus grand sur le fusil à pompe. L'autre choix : dis-moi lequel tu préfères (position dans la vidéo).

## Subtasks
1. task-440-01-flash-system (vault/reference/tasks/task-440-muzzle-flash-rework/task-440-01-flash-system.md) — bp: The muzzle flash system and its soft material have the new look, larger on the shotgun.
2. task-440-02-doc (vault/reference/tasks/task-440-muzzle-flash-rework/task-440-02-doc.md) — doc: Document the new values.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-440-01-flash-system]] — step 1, bp.
- [[task-440-02-doc]] — step 2, doc.
