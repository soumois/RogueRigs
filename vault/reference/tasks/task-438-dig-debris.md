---
id: task-438-dig-debris
description: "Global task: A block broken by the drone throws a short burst of debris of the block colour that falls and disappears."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Debris when digging

- request: Owner 2026-11-05 (verbatim, whole list): "Voici une liste de tâche, découpe les avec le planneur, pose les questions et puis lance l'agent executor par lot de tâche ou par tâche selon ce que tu penses qu'il sera le mieux : * Retire le flash sur le robot quand ils sont touchés. * Améliorer la particule de mort du robot * La particule d'impact de tir doit s'activer aussi quand tu touches un mur. * Il faut aussi une particule de débris de terre lorsqu'on creuse. * Lors d'une explosion, si des murs sont détruit, les débris doivent aller dans la direction opposée d'ou vient l'explosion * Il faut retravailler le muzzle flash * Il faut retravailler l'effet de level up ( un text qui pop comme les dégâts, mais pas d'effet d'arc et en couleur dorée ) + particule au sol + ennemi légèrement repoussé ( la particule doit donner cet effet ) * La particule de sous les hélices de l'extracteur font plus pensé à de la pluie qu'autre chose, il faut modifié ça * Il faudrait aussi représenter le souffle des hélices de l'extracteur au sol ( qui diminue en fonction de sa hauteur ( et elle disparait quand il est assez haut ( + de 4m ) * Les hélices projectent de l'ombre de l'extracteur, retire ça * il faudrait que la light du joueur projette de l'ombre des murs ( seulement )" This task is the point "Il faut aussi une particule de débris de terre lorsqu'on creuse."
- goal: Digging visibly throws debris: each block the drone breaks bursts into pieces.
- acceptance: A1 in game, each block broken by the drone laser throws a one-shot burst of debris that is easy to see at the game camera distance and falls to the floor; A2 colour of the block: lavender rock, violet Duskite, red heal; A3 at most 12 CPU sprites per block, gone within 0.8 s, nothing stays on the floor; A4 through the Niagara pool or one shared system, skipped when nothing is free; A5 the looping effect at the laser impact, dig speed and block rules unchanged, the extractor footprint clear and explosions do not play it; A6 No capture and no stress run in this task unless the owner asks: he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it.
- gdd_check: Read drone, bp-dig-impact, color-code, art-style-guide A14, explosions. NS_DigImpact already plays Chips (2.5 to 4.5 uu sprites, very small at a 1400 uu camera arm) and Dust at the laser point, so the point has two readings: Q6. "Terre" against the code colour of blocks: Q7. A14 forbids rubble on the floor, hence the disappearance. Targeted read of Source (Grep, because Q6 depends on it): no class holding a Niagara system for a broken block was found; Blueprints not checked. drone edited at confirmation.
- confirmed_by_owner: no (plan and questions handed back 2026-11-05)
- milestone: M2
- next_step: task-438-01-trigger-on-break
- started: open
- waiting_for: owner answers to the questions of the 2026-11-05 FX and light list (Q1 to Q17, "ok" accepts every default)
- result: open

## Questions
Asked 2026-11-05, each with a default; open until the owner answers.
- Q6 Il existe déjà des petits éclats (2,5 à 4,5 cm, presque invisibles vus de la caméra) au point du laser. Que veux-tu ? Default: un nouvel effet, chaque bloc cassé par le drone lance une gerbe de débris qui retombent et disparaissent en moins de 0,8 s, au plus 12 particules, et l'effet du laser ne change pas. L'autre choix : pas de gerbe, seulement grossir les éclats du laser pendant qu'il coupe.
- Q7 « Terre », c'est quelle couleur ? Default: la couleur du bloc cassé (lavande pour la roche, violet pour le Duskite, rouge pour le minerai de soin), comme le code couleur. L'autre choix : un brun terre, qui n'existe nulle part ailleurs dans l'arène.

## Subtasks
1. task-438-01-trigger-on-break (vault/reference/tasks/task-438-dig-debris/task-438-01-trigger-on-break.md) — cpp: A block broken by the drone requests one debris burst at the block with the block colour, bounded.
2. task-438-02-debris-system (vault/reference/tasks/task-438-dig-debris/task-438-02-debris-system.md) — bp: The one-shot dig debris Niagara system exists and is the one the game plays.
3. task-438-03-doc (vault/reference/tasks/task-438-dig-debris/task-438-03-doc.md) — doc: Document the burst and its bound.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-438-01-trigger-on-break]] — step 1, cpp.
- [[task-438-02-debris-system]] — step 2, bp.
- [[task-438-03-doc]] — step 3, doc.
