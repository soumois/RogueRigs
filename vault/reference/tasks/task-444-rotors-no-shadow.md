---
id: task-444-rotors-no-shadow
description: "Global task: The whole extractor, hull and rotors, casts no shadow."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# The whole extractor casts no shadow

- request: Owner 2026-11-05 (verbatim, whole list): "Voici une liste de tâche, découpe les avec le planneur, pose les questions et puis lance l'agent executor par lot de tâche ou par tâche selon ce que tu penses qu'il sera le mieux : * Retire le flash sur le robot quand ils sont touchés. * Améliorer la particule de mort du robot * La particule d'impact de tir doit s'activer aussi quand tu touches un mur. * Il faut aussi une particule de débris de terre lorsqu'on creuse. * Lors d'une explosion, si des murs sont détruit, les débris doivent aller dans la direction opposée d'ou vient l'explosion * Il faut retravailler le muzzle flash * Il faut retravailler l'effet de level up ( un text qui pop comme les dégâts, mais pas d'effet d'arc et en couleur dorée ) + particule au sol + ennemi légèrement repoussé ( la particule doit donner cet effet ) * La particule de sous les hélices de l'extracteur font plus pensé à de la pluie qu'autre chose, il faut modifié ça * Il faudrait aussi représenter le souffle des hélices de l'extracteur au sol ( qui diminue en fonction de sa hauteur ( et elle disparait quand il est assez haut ( + de 4m ) * Les hélices projectent de l'ombre de l'extracteur, retire ça * il faudrait que la light du joueur projette de l'ombre des murs ( seulement )" This task is the point "Les hélices projectent de l'ombre de l'extracteur, retire ça". Owner answer 2026-11-05 (relayed by the main session, verbatim): "Q2 - Remesurer les tokens, je n'ai aucune idée du couts pour les tâches 432/433 et 434 par exemple. Q3 - C'est un bete nuage de fumée, j'aimerais des particules un peu plus cartoon pour que cela flash ( voir image 1 comme exemple ). Q5 - Les murs que l'on peut creuser. Tu avais marqué dans ton rapport de la précedante tâche qu'il n'y avait pas d'éteincelles lorsqu'on tire sur la roche ou du minerai. Je veux changer ça. Q9 - Il est trop basique, tu peux voir dans la vidéo ( tu peux voir de 13:28 et 13:33 ). Q14 : l'extractor entier ne doit pas projeter d'ombre ( tu peux voir sur l'image 2 qu'on voit l'ombre de deux hélices ). Q16 j 'ai déjà bougé moi même la hauteur de la lumiére. Q17 : Non, mais je veux bien les couts en tokens" = the plan and the defaults of Q14 accepted.
- goal: No part of the extractor casts a shadow, on itself or on the floor.
- acceptance: A1 in game, neither the hull nor the four rotors cast a shadow on the floor or on the hull, from any light; the two rotor shadows of Art/Source/extractor-fx/20261105-owner-rotor-shadows-and-streaks.webp are gone; A2 the shadows of walls and barrels are unchanged; A3 arrival and extraction extractor alike, including during takeoff; A4 rotor spin and look unchanged; A5 No capture and no stress run in this task unless the owner asks: he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it. The report also gives the token cost of this task, per subtask and in total, from cost-log (units, turns, runs, resumes): owner Q2 and Q17, 2026-11-05.
- gdd_check: Read extraction, bp-extraction-shuttle, lights-no-shadows (objectives are not covered by the C++ no-shadow rule, flags as authored). No conflict. Q14 answered with Art/Source/extractor-fx/20261105-owner-rotor-shadows-and-streaks.webp: scope widened from the rotors to the whole extractor. extraction edited at confirmation.
- confirmed_by_owner: yes (2026-11-05, "Q2 - Remesurer les tokens, je n'ai aucune idée du couts pour les tâches 432/433 et 434 par exemple. Q3 - C'est un bete nuage de fumée, j'aimerais des particules un peu plus cartoon pour que cela flash ( voir image 1 comme exemple ). Q5 - Les murs que l'on peut creuser. Tu avais marqué dans ton rapport de la précedante tâche qu'il n'y avait pas d'éteincelles lorsqu'on tire sur la roche ou du minerai. Je veux changer ça. Q9 - Il est trop basique, tu peux voir dans la vidéo ( tu peux voir de 13:28 et 13:33 ). Q14 : l'extractor entier ne doit pas projeter d'ombre ( tu peux voir sur l'image 2 qu'on voit l'ombre de deux hélices ). Q16 j 'ai déjà bougé moi même la hauteur de la lumiére. Q17 : Non, mais je veux bien les couts en tokens")
- milestone: M2
- next_step: task-444-01-rotor-shadow-off
- started: 2026-11-05T21:07:22Z
- waiting_for: none
- result: open

## Questions
Decided 2026-11-05 (owner "Q2 - Remesurer les tokens, je n'ai aucune idée du couts pour les tâches 432/433 et 434 par exemple. Q3 - C'est un bete nuage de fumée, j'aimerais des particules un peu plus cartoon pour que cela flash ( voir image 1 comme exemple ). Q5 - Les murs que l'on peut creuser. Tu avais marqué dans ton rapport de la précedante tâche qu'il n'y avait pas d'éteincelles lorsqu'on tire sur la roche ou du minerai. Je veux changer ça. Q9 - Il est trop basique, tu peux voir dans la vidéo ( tu peux voir de 13:28 et 13:33 ). Q14 : l'extractor entier ne doit pas projeter d'ombre ( tu peux voir sur l'image 2 qu'on voit l'ombre de deux hélices ). Q16 j 'ai déjà bougé moi même la hauteur de la lumiére. Q17 : Non, mais je veux bien les couts en tokens"): every default below is accepted; values provisional, owner 2026-11-05.
- Q14 Quelles ombres retirer ? Default (selon la réponse du propriétaire, 2026-11-05 : « Q14 : l'extractor entier ne doit pas projeter d'ombre ( tu peux voir sur l'image 2 qu'on voit l'ombre de deux hélices ) ») : ni la coque ni les hélices ne projettent d'ombre.

## Subtasks
1. task-444-01-rotor-shadow-off (vault/reference/tasks/task-444-rotors-no-shadow/task-444-01-rotor-shadow-off.md) — bp: The hull and the four rotors cast no shadow.
2. task-444-02-doc (vault/reference/tasks/task-444-rotors-no-shadow/task-444-02-doc.md) — doc: Record the rotor shadow flags.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-444-01-rotor-shadow-off]] — step 1, bp.
- [[task-444-02-doc]] — step 2, doc.
