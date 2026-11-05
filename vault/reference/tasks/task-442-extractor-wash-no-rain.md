---
id: task-442-extractor-wash-no-rain
description: "Global task: The thin downward streaks under the extractor rotors, read as rain, become soft faint air swirls."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Extractor rotor wash that does not read as rain

- request: Owner 2026-11-05 (verbatim, whole list): "Voici une liste de tâche, découpe les avec le planneur, pose les questions et puis lance l'agent executor par lot de tâche ou par tâche selon ce que tu penses qu'il sera le mieux : * Retire le flash sur le robot quand ils sont touchés. * Améliorer la particule de mort du robot * La particule d'impact de tir doit s'activer aussi quand tu touches un mur. * Il faut aussi une particule de débris de terre lorsqu'on creuse. * Lors d'une explosion, si des murs sont détruit, les débris doivent aller dans la direction opposée d'ou vient l'explosion * Il faut retravailler le muzzle flash * Il faut retravailler l'effet de level up ( un text qui pop comme les dégâts, mais pas d'effet d'arc et en couleur dorée ) + particule au sol + ennemi légèrement repoussé ( la particule doit donner cet effet ) * La particule de sous les hélices de l'extracteur font plus pensé à de la pluie qu'autre chose, il faut modifié ça * Il faudrait aussi représenter le souffle des hélices de l'extracteur au sol ( qui diminue en fonction de sa hauteur ( et elle disparait quand il est assez haut ( + de 4m ) * Les hélices projectent de l'ombre de l'extracteur, retire ça * il faudrait que la light du joueur projette de l'ombre des murs ( seulement )" This task is the point "La particule de sous les hélices de l'extracteur font plus pensé à de la pluie qu'autre chose, il faut modifié ça". Owner answer 2026-11-05 (relayed by the main session, verbatim): "Q2 - Remesurer les tokens, je n'ai aucune idée du couts pour les tâches 432/433 et 434 par exemple. Q3 - C'est un bete nuage de fumée, j'aimerais des particules un peu plus cartoon pour que cela flash ( voir image 1 comme exemple ). Q5 - Les murs que l'on peut creuser. Tu avais marqué dans ton rapport de la précedante tâche qu'il n'y avait pas d'éteincelles lorsqu'on tire sur la roche ou du minerai. Je veux changer ça. Q9 - Il est trop basique, tu peux voir dans la vidéo ( tu peux voir de 13:28 et 13:33 ). Q14 : l'extractor entier ne doit pas projeter d'ombre ( tu peux voir sur l'image 2 qu'on voit l'ombre de deux hélices ). Q16 j 'ai déjà bougé moi même la hauteur de la lumiére. Q17 : Non, mais je veux bien les couts en tokens" = the plan and the defaults of Q12 accepted.
- goal: The air under the rotors reads as air pushed down, never as rain.
- acceptance: A1 in game, under each rotor no thin vertical lines remain; a few large soft very faint swirls go down and spread; A2 same pale grey-beige, no code colour; A3 still driven by the rotor speed, stronger at takeoff; A4 the floor dust stays as it is in this task; rotor spin and takeoff unchanged; A5 live particles no more than the emitter it replaces, about 30 per rotor; A6 No capture and no stress run in this task unless the owner asks: he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it. The report also gives the token cost of this task, per subtask and in total, from cost-log (units, turns, runs, resumes): owner Q2 and Q17, 2026-11-05.
- gdd_check: Read extraction, bp-extractor-rotor-wash, color-code. No conflict; the "downward air streaks" of extraction edited at confirmation. Same asset as task-443, which runs after.
- confirmed_by_owner: yes (2026-11-05, "Q2 - Remesurer les tokens, je n'ai aucune idée du couts pour les tâches 432/433 et 434 par exemple. Q3 - C'est un bete nuage de fumée, j'aimerais des particules un peu plus cartoon pour que cela flash ( voir image 1 comme exemple ). Q5 - Les murs que l'on peut creuser. Tu avais marqué dans ton rapport de la précedante tâche qu'il n'y avait pas d'éteincelles lorsqu'on tire sur la roche ou du minerai. Je veux changer ça. Q9 - Il est trop basique, tu peux voir dans la vidéo ( tu peux voir de 13:28 et 13:33 ). Q14 : l'extractor entier ne doit pas projeter d'ombre ( tu peux voir sur l'image 2 qu'on voit l'ombre de deux hélices ). Q16 j 'ai déjà bougé moi même la hauteur de la lumiére. Q17 : Non, mais je veux bien les couts en tokens")
- milestone: M2
- next_step: task-442-02-doc
- started: 2026-11-05T20:35:50Z
- waiting_for: none
- result: open

## Questions
Decided 2026-11-05 (owner "Q2 - Remesurer les tokens, je n'ai aucune idée du couts pour les tâches 432/433 et 434 par exemple. Q3 - C'est un bete nuage de fumée, j'aimerais des particules un peu plus cartoon pour que cela flash ( voir image 1 comme exemple ). Q5 - Les murs que l'on peut creuser. Tu avais marqué dans ton rapport de la précedante tâche qu'il n'y avait pas d'éteincelles lorsqu'on tire sur la roche ou du minerai. Je veux changer ça. Q9 - Il est trop basique, tu peux voir dans la vidéo ( tu peux voir de 13:28 et 13:33 ). Q14 : l'extractor entier ne doit pas projeter d'ombre ( tu peux voir sur l'image 2 qu'on voit l'ombre de deux hélices ). Q16 j 'ai déjà bougé moi même la hauteur de la lumiére. Q17 : Non, mais je veux bien les couts en tokens"): every default below is accepted; values provisional, owner 2026-11-05.
- Q12 Par quoi remplacer les traits qui font pluie ? Default: quelques grandes volutes douces et très transparentes qui descendent en tournant et s'écartent, même gris-beige pâle. L'autre choix : supprimer complètement ces particules et ne garder que le souffle au sol (tâche suivante).

## Subtasks
1. task-442-01-swirls-emitter (vault/reference/tasks/task-442-extractor-wash-no-rain/task-442-01-swirls-emitter.md) — bp: The streaks emitter of the rotor wash is replaced by soft faint swirls.
2. task-442-02-doc (vault/reference/tasks/task-442-extractor-wash-no-rain/task-442-02-doc.md) — doc: Document the new emitter.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-442-01-swirls-emitter]] — step 1, bp.
- [[task-442-02-doc]] — step 2, doc.
