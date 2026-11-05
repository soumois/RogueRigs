---
id: task-445-miner-light-wall-shadows
description: "Global task: The miner floor and wall light casts the shadows of rock walls, and of nothing else."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Miner light casts wall shadows only

- request: Owner 2026-11-05 (verbatim, whole list): "Voici une liste de tâche, découpe les avec le planneur, pose les questions et puis lance l'agent executor par lot de tâche ou par tâche selon ce que tu penses qu'il sera le mieux : * Retire le flash sur le robot quand ils sont touchés. * Améliorer la particule de mort du robot * La particule d'impact de tir doit s'activer aussi quand tu touches un mur. * Il faut aussi une particule de débris de terre lorsqu'on creuse. * Lors d'une explosion, si des murs sont détruit, les débris doivent aller dans la direction opposée d'ou vient l'explosion * Il faut retravailler le muzzle flash * Il faut retravailler l'effet de level up ( un text qui pop comme les dégâts, mais pas d'effet d'arc et en couleur dorée ) + particule au sol + ennemi légèrement repoussé ( la particule doit donner cet effet ) * La particule de sous les hélices de l'extracteur font plus pensé à de la pluie qu'autre chose, il faut modifié ça * Il faudrait aussi représenter le souffle des hélices de l'extracteur au sol ( qui diminue en fonction de sa hauteur ( et elle disparait quand il est assez haut ( + de 4m ) * Les hélices projectent de l'ombre de l'extracteur, retire ça * il faudrait que la light du joueur projette de l'ombre des murs ( seulement )" This task is the point "il faudrait que la light du joueur projette de l'ombre des murs ( seulement )".
- goal: Walls cast a moving shadow away from the miner; nothing else casts from his light.
- acceptance: A1 in game, around the miner, rock, ore and heal ore blocks cast a clearly visible shadow on the floor, moving as he moves; A2 robots, miner, drone, pickups, barrels, chest and ore crystals cast no shadow from this light; A3 the light sits just above the wall tops, about 1.5 times the wall height, its pool stays about 400 uu and the floor under the miner below about 205/255, values retuned by calculation, provisional, owner tunes in BP_Miner; A4 sun, sky, exposure and screen darkener unchanged; A5 what it lights is unchanged (floor and blocks only); A6 No capture and no stress run in this task unless the owner asks: he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it. The report gives the old cost figures from perf-risks and how to turn the shadow off.
- gdd_check: Read visual-style, task-425 (Q1 "no shadow" accepted by the owner 2026-11-04: reversed here), bp-miner, lights-no-shadows, lights-dark-ambient, perf-risks. Light meant: FloorWallLight of task-425, the older PlayerLight is removed. visual-style edited at confirmation. Two readings of "seulement": Q15. The light sits about 450 uu above the floor and blocks are low, so shadows would be short: Q16. Cost measured on the old PlayerLight: +2.5 to 3 ms at 1080p at radius 3000 uu; about +0.65 ms shadow depths near radius 640 uu; the current radius is 650 uu, not measured: Q17.
- confirmed_by_owner: no (plan and questions handed back 2026-11-05)
- milestone: M2
- next_step: task-445-01-light-shadows
- started: open
- waiting_for: owner answers to the questions of the 2026-11-05 FX and light list (Q1 to Q17, "ok" accepts every default)
- result: open

## Questions
Asked 2026-11-05, each with a default; open until the owner answers.
- Q15 « Seulement » veut dire quoi ? Default: parmi tout ce que cette lumière éclaire, seuls les murs (roche, minerai, minerai de soin) projettent une ombre ; robots, mineur, drone, barils et objets n'en projettent pas. Le soleil et le ciel gardent leurs ombres actuelles. L'autre choix : couper aussi les ombres du soleil et du ciel, pour que seule la lumière du joueur fasse des ombres dans tout le jeu ; c'était ta référence DRGS du 20 octobre.
- Q16 Cette lumière est à environ 4,5 m du sol, les murs sont bas, donc les ombres seraient courtes. Default: la descendre juste au-dessus du sommet des murs, environ 1,5 fois leur hauteur, pour des ombres qui se lisent, et recalculer l'intensité pour garder la flaque d'environ 400 uu sans brûler le sol ; tu ajustes la hauteur dans BP_Miner. L'autre choix : ne pas toucher à sa position, ombres courtes.
- Q17 Coût : l'ancienne lumière du joueur avec ombres coûtait 2,5 à 3 ms en 1080p avec un rayon de 3000 uu, et l'ombre seule environ 0,65 ms près de 640 uu. Celle-ci a un rayon de 650 uu. Default: pas de mesure ici (voir Q2) ; le rapport te dit comment couper l'ombre si tu vois une baisse en jeu. L'autre choix : une mesure GPU avec et sans ombre.

## Subtasks
1. task-445-01-light-shadows (vault/reference/tasks/task-445-miner-light-wall-shadows/task-445-01-light-shadows.md) — bp: FloorWallLight casts shadows and sits just above the wall tops, its pool kept by retuned values.
2. task-445-02-casters-walls-only (vault/reference/tasks/task-445-miner-light-wall-shadows/task-445-02-casters-walls-only.md) — cpp: From this light, only rock, ore and heal ore blocks cast shadows; anything found casting that should not is corrected.
3. task-445-03-doc (vault/reference/tasks/task-445-miner-light-wall-shadows/task-445-03-doc.md) — doc: Document the shadow, the values and how to turn it off.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-445-01-light-shadows]] — step 1, bp.
- [[task-445-02-casters-walls-only]] — step 2, cpp.
- [[task-445-03-doc]] — step 3, doc.
