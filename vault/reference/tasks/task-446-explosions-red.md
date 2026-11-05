---
id: task-446-explosions-red
description: "Global task: The barrel and kamikaze explosion burst gets red, as the owner now allows and expects of an explosion."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Red in the barrel and kamikaze explosions

- request: Owner 2026-11-05, answer verbatim: "Q18 : Il ne faut être si rigide sur le code couleur, une explosion dans l'esprit collective a aussi du rouge. Le code couleur ne peut pas non plus aller à l'encontre de ce qu'attend un joueur de l'explosion". Then, relayed by the main session, answering "Baril et kamikaze : leurs explosions n'ont toujours pas de rouge. La règle le permet maintenant, mais les recolorer serait une tâche de plus. Dis-moi si tu la veux.", verbatim: "1. applique".
- goal: The barrel and kamikaze explosions show the red a player expects of an explosion.
- acceptance: A1 in game, the barrel and kamikaze explosion burst shows red-orange at the edge of its fireball and dark red in its smoke, easy to see; A2 its white and yellow flash and its yellow-orange sparks stay; A3 the orange sphere and the orange ground ring are unchanged, and so are the danger circles of ground-telegraph; A4 particle counts, sizes, timings, radii, damage and camera shake are unchanged; A5 both rows keep playing the same class, no second child; A6 No capture and no stress run in this task unless the owner asks: he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it. The report also gives the token cost of this task, per subtask and in total, from cost-log (units, turns, runs, resumes).
- gdd_check: Read bp-barrel-explosion, bp-explosion-fx-barrel, bp-explosion-fx, bp-explosion-shockwave, explosions, readability-rules, color-code, task-434. No conflict since the owner softened the colour rules (readability-rules, 2026-11-05). Both rows play BP_ExplosionFX_Barrel, so one change covers both: NS_BarrelExplosion is changed; MI_GB_Explosion (sphere, also in the unplayed parent) and MI_ExplosionShockwave (ring, the danger orange shared with the telegraphs) stay orange by the default of Q20. Task-434 A4 "no red" stays as a closed record. "Applique" leaves open recolour or full cartoon redo: Q20. A10 and explosions edited at confirmation.
- confirmed_by_owner: no (plan and questions handed back 2026-11-05)
- milestone: M2
- next_step: task-446-01-burst-colours
- started: open
- waiting_for: owner answer to Q20 ("ok" accepts the default)
- result: open

## Questions
Asked 2026-11-05, each with a default; open until the owner answers.
- Q20 Rouge sur les explosions du baril et du kamikaze : jusqu'où ? Default: on recolore seulement la gerbe de particules : bords de boule de feu rouge-orangé et rouge sombre dans la fumée, d'après les couleurs de ton image ; le flash et les étincelles restent ; la sphère et l'anneau au sol restent orange, comme les cercles de danger ; mêmes tailles et même durée. L'autre choix : refaire toute l'explosion en style cartoon comme le robot, ce qui est une tâche plus grosse.

## Subtasks
1. task-446-01-burst-colours (vault/reference/tasks/task-446-explosions-red/task-446-01-burst-colours.md) — bp: The explosion burst holds red at the fireball edges and in the smoke.
2. task-446-02-doc (vault/reference/tasks/task-446-explosions-red/task-446-02-doc.md) — doc: Document the colours and remove the "no red" statements that no longer hold.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-446-01-burst-colours]] — step 1, bp.
- [[task-446-02-doc]] — step 2, doc.
