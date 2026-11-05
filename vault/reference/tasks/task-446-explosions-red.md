---
id: task-446-explosions-red
description: "Global task: The whole barrel and kamikaze explosion, sphere, burst and ring, is redone in the cartoon style of the owner reference."
parent: tasks-to-do-index
kind: task
status: todo
---
# Barrel and kamikaze explosions in cartoon style

- request: Owner 2026-11-05, answer verbatim: "Q18 : Il ne faut être si rigide sur le code couleur, une explosion dans l'esprit collective a aussi du rouge. Le code couleur ne peut pas non plus aller à l'encontre de ce qu'attend un joueur de l'explosion". Then, relayed by the main session, answering "Baril et kamikaze : leurs explosions n'ont toujours pas de rouge. La règle le permet maintenant, mais les recolorer serait une tâche de plus. Dis-moi si tu la veux.", verbatim: "1. applique". Owner answer 2026-11-05 (relayed by the main session, verbatim): "Q20- refaire tout l'explosion en style cartoon" = the plan and the defaults of Q20 accepted.
- goal: The barrel and kamikaze explosions look like the cartoon explosion the owner showed, in the same style as the robot death flash, at their own scale.
- acceptance: A1 in game, the barrel and kamikaze explosion reads as the cartoon explosion of Art/Source/robot-death-fx/20261105-owner-ref-cartoon-explosion.png (a reference for shape and style, not an asset to ship), and as the same family as the robot death flash of task-436: small flash, white and yellow core growing with jagged cel-shaded edges in flat colour bands, orange and red-orange outer bands, chunky blobs and thin straight lines thrown outward, then a hollow ring burning out into dark red puffs; A2 "tout" is read as all three parts: the sphere, the burst and the ground ring have this style, no soft round glow left as the main look; A3 the rules stay: the visible part is about 5 % larger than the collision radius and stays 0.1 s at its final state, the ring still grows to the radius, so the player is never hit outside what is shown; A4 the danger circles before an explosion (ground-telegraph), the barrel fuse and the kamikaze core pulse are unchanged; A5 both rows keep playing one class, each at its own radius; A6 at most 95 CPU sprites per explosion as today, gone within the 1.5 s the actor lives, nothing stays on the floor; A7 damage, radii, chains and camera shake unchanged, explosion tests pass, only expectations this change breaks are updated; A8 No capture and no stress run in this task unless the owner asks: he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it. The report also gives the token cost of this task, per subtask and in total, from cost-log (units, turns, runs, resumes).
- gdd_check: Read bp-barrel-explosion, bp-explosion-fx-barrel, bp-explosion-fx, bp-explosion-shockwave, explosions, ground-telegraph, readability-rules, color-code, task-434. No conflict since the owner softened the colour rules (readability-rules, 2026-11-05). Q20: his "tout" replaces the recolour default. Both rows play BP_ExplosionFX_Barrel. The ring material is a copy of the telegraph material with its own instance, so restyling it need not touch the telegraphs; the ring is read as part of "tout", its orange staying inside the reference palette, nothing open. The "Visible explosion" rule is a game rule and stays (rules versus rendering). A10 and explosions edited at confirmation. Task-434 A4 "no red" stays as a closed record.
- confirmed_by_owner: yes (2026-11-05, "Q20- refaire tout l'explosion en style cartoon")
- milestone: M2
- next_step: task-446-01-cartoon-burst
- started: open
- waiting_for: none
- result: open

## Questions
Decided 2026-11-05 (owner "Q20- refaire tout l'explosion en style cartoon"): every default below is accepted; values provisional, owner 2026-11-05.
- Q20 Rouge sur les explosions du baril et du kamikaze : jusqu'où ? Default (selon la réponse du propriétaire, 2026-11-05 : « Q20- refaire tout l'explosion en style cartoon ») : toute l'explosion, sphère, gerbe et anneau au sol, en style cartoon d'après Art/Source/robot-death-fx/20261105-owner-ref-cartoon-explosion.png, même famille que la mort du robot ; le rayon reste montré comme aujourd'hui et les cercles de danger avant l'explosion ne changent pas.

## Subtasks
1. task-446-01-cartoon-burst (vault/reference/tasks/task-446-explosions-red/task-446-01-cartoon-burst.md) — bp: Representative slice: the burst and its material have the cartoon look, in the style of the robot death flash.
2. task-446-02-cartoon-sphere (vault/reference/tasks/task-446-explosions-red/task-446-02-cartoon-sphere.md) — bp: The expanding sphere of the explosion has the cartoon look and still shows the radius.
3. task-446-03-cartoon-ring (vault/reference/tasks/task-446-explosions-red/task-446-03-cartoon-ring.md) — bp: The ground ring has the cartoon look and still grows to the radius; the telegraphs do not change.
4. task-446-04-doc (vault/reference/tasks/task-446-explosions-red/task-446-04-doc.md) — doc: Document the new look and remove the statements that no longer hold.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-446-01-cartoon-burst]] — step 1, bp.
- [[task-446-02-cartoon-sphere]] — step 2, bp.
- [[task-446-03-cartoon-ring]] — step 3, bp.
- [[task-446-04-doc]] — step 4, doc.
