---
id: task-444-rotors-no-shadow
description: "Global task: The four extractor rotors cast no shadow; the hull keeps its own."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Extractor rotors cast no shadow

- request: Owner 2026-11-05 (verbatim, whole list): "Voici une liste de tâche, découpe les avec le planneur, pose les questions et puis lance l'agent executor par lot de tâche ou par tâche selon ce que tu penses qu'il sera le mieux : * Retire le flash sur le robot quand ils sont touchés. * Améliorer la particule de mort du robot * La particule d'impact de tir doit s'activer aussi quand tu touches un mur. * Il faut aussi une particule de débris de terre lorsqu'on creuse. * Lors d'une explosion, si des murs sont détruit, les débris doivent aller dans la direction opposée d'ou vient l'explosion * Il faut retravailler le muzzle flash * Il faut retravailler l'effet de level up ( un text qui pop comme les dégâts, mais pas d'effet d'arc et en couleur dorée ) + particule au sol + ennemi légèrement repoussé ( la particule doit donner cet effet ) * La particule de sous les hélices de l'extracteur font plus pensé à de la pluie qu'autre chose, il faut modifié ça * Il faudrait aussi représenter le souffle des hélices de l'extracteur au sol ( qui diminue en fonction de sa hauteur ( et elle disparait quand il est assez haut ( + de 4m ) * Les hélices projectent de l'ombre de l'extracteur, retire ça * il faudrait que la light du joueur projette de l'ombre des murs ( seulement )" This task is the point "Les hélices projectent de l'ombre de l'extracteur, retire ça".
- goal: No spinning rotor shadow appears on the extractor or on the floor.
- acceptance: A1 in game, the four rotors cast no shadow on the hull or on the floor, from any light; A2 the hull keeps its shadow; A3 arrival and extraction extractor alike; A4 rotor spin and look unchanged; A5 No capture and no stress run in this task unless the owner asks: he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it.
- gdd_check: Read extraction, bp-extraction-shuttle, lights-no-shadows (objectives are not covered by the C++ no-shadow rule, flags as authored). No conflict. extraction edited at confirmation.
- confirmed_by_owner: no (plan and questions handed back 2026-11-05)
- milestone: M2
- next_step: task-444-01-rotor-shadow-off
- started: open
- waiting_for: owner answers to the questions of the 2026-11-05 FX and light list (Q1 to Q17, "ok" accepts every default)
- result: open

## Questions
Asked 2026-11-05, each with a default; open until the owner answers.
- Q14 Default: seules les quatre hélices ne projettent plus d'ombre, ni sur la coque ni au sol ; la coque garde son ombre.

## Subtasks
1. task-444-01-rotor-shadow-off (vault/reference/tasks/task-444-rotors-no-shadow/task-444-01-rotor-shadow-off.md) — bp: The four rotors cast no shadow, the hull does.
2. task-444-02-doc (vault/reference/tasks/task-444-rotors-no-shadow/task-444-02-doc.md) — doc: Record the rotor shadow flags.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-444-01-rotor-shadow-off]] — step 1, bp.
- [[task-444-02-doc]] — step 2, doc.
