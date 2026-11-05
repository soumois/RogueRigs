---
id: task-442-extractor-wash-no-rain
description: "Global task: The thin downward streaks under the extractor rotors, read as rain, become soft faint air swirls."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Extractor rotor wash that does not read as rain

- request: Owner 2026-11-05 (verbatim, whole list): "Voici une liste de tâche, découpe les avec le planneur, pose les questions et puis lance l'agent executor par lot de tâche ou par tâche selon ce que tu penses qu'il sera le mieux : * Retire le flash sur le robot quand ils sont touchés. * Améliorer la particule de mort du robot * La particule d'impact de tir doit s'activer aussi quand tu touches un mur. * Il faut aussi une particule de débris de terre lorsqu'on creuse. * Lors d'une explosion, si des murs sont détruit, les débris doivent aller dans la direction opposée d'ou vient l'explosion * Il faut retravailler le muzzle flash * Il faut retravailler l'effet de level up ( un text qui pop comme les dégâts, mais pas d'effet d'arc et en couleur dorée ) + particule au sol + ennemi légèrement repoussé ( la particule doit donner cet effet ) * La particule de sous les hélices de l'extracteur font plus pensé à de la pluie qu'autre chose, il faut modifié ça * Il faudrait aussi représenter le souffle des hélices de l'extracteur au sol ( qui diminue en fonction de sa hauteur ( et elle disparait quand il est assez haut ( + de 4m ) * Les hélices projectent de l'ombre de l'extracteur, retire ça * il faudrait que la light du joueur projette de l'ombre des murs ( seulement )" This task is the point "La particule de sous les hélices de l'extracteur font plus pensé à de la pluie qu'autre chose, il faut modifié ça".
- goal: The air under the rotors reads as air pushed down, never as rain.
- acceptance: A1 in game, under each rotor no thin vertical lines remain; a few large soft very faint swirls go down and spread; A2 same pale grey-beige, no code colour; A3 still driven by the rotor speed, stronger at takeoff; A4 the floor dust stays as it is in this task; rotor spin and takeoff unchanged; A5 live particles no more than the emitter it replaces, about 30 per rotor; A6 No capture and no stress run in this task unless the owner asks: he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it.
- gdd_check: Read extraction, bp-extractor-rotor-wash, color-code. No conflict; the "downward air streaks" of extraction edited at confirmation. Same asset as task-443, which runs after.
- confirmed_by_owner: no (plan and questions handed back 2026-11-05)
- milestone: M2
- next_step: task-442-01-swirls-emitter
- started: open
- waiting_for: owner answers to the questions of the 2026-11-05 FX and light list (Q1 to Q17, "ok" accepts every default)
- result: open

## Questions
Asked 2026-11-05, each with a default; open until the owner answers.
- Q12 Par quoi remplacer les traits qui font pluie ? Default: quelques grandes volutes douces et très transparentes qui descendent en tournant et s'écartent, même gris-beige pâle. L'autre choix : supprimer complètement ces particules et ne garder que le souffle au sol (tâche suivante).

## Subtasks
1. task-442-01-swirls-emitter (vault/reference/tasks/task-442-extractor-wash-no-rain/task-442-01-swirls-emitter.md) — bp: The streaks emitter of the rotor wash is replaced by soft faint swirls.
2. task-442-02-doc (vault/reference/tasks/task-442-extractor-wash-no-rain/task-442-02-doc.md) — doc: Document the new emitter.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-442-01-swirls-emitter]] — step 1, bp.
- [[task-442-02-doc]] — step 2, doc.
