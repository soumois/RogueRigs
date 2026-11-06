---
id: task-465-shot-impact-burst
description: "Global task: Player shot impacts become a flat cartoon burst of spikes and triangles from the owner reference: white for projectiles, green for lasers."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Player shot impact burst

- request: Owner 2026-11-05, verbatim: "Pour l'impact on pourrait aller sur quelque chose comme ça ( blanc pour projectile, vert pour le laser )", with a reference image (kept as Art/Source/impact-burst/owner-ref-impact-burst-2026-11-05.png: stylised cartoon hit burst on a dark background, bright glowing core, radial star of sharp white and yellow-orange spikes and shards, soft orange glow halo, small triangles flying outward, solid white, solid black, outlined yellow, outlined black; flat, graphic, angular shapes). Owner answer 2026-11-06 (relayed by the main session, verbatim): "prépare la tâche sans lancer, j'accepte les propostions." = the plan and the defaults of Q1, Q2, Q3, Q4, Q5, Q6, Q7 accepted. Owner launch order (relayed by the main session, verbatim): "Lance les task to do dans l'ordre" = launch now; this lifts the earlier "do not launch" instruction.
- goal: Every impact of a player shot reads as a punchy flat cartoon burst in the style of the owner reference, white for projectile weapons and green for laser weapons, without hiding the fight.
- acceptance: A1 in a -game run a burst rifle shot that hits shows, at the hit point, a flat cartoon burst in the style of the reference (bright core, radial star of sharp spikes, soft glow, small solid and outlined triangles flying out, gone in a fraction of a second), in the varied green of task-464 (pale core, bright green, dark green glow), no longer the old sparks; the reference is a style example, not an asset to copy; A2 turret shot impacts show the same green burst, sized to the turret shot (default of Q4); A3 shotgun pellet impacts show the same burst in white with the warm gold-orange accents of the reference (default of Q2); A4 the burst appears wherever today's impact sparks appear (robot, rock, barrel) (default of Q5); A5 the burst stays small and short: about the size of today's sparks, at most twice; with a full burst-rifle volley on a group of robots the robots and the damage numbers stay readable and the Shooter is still spotted in under a second (default of Q3); A6 enemy shot impacts, the mining laser and the aim laser unchanged (default of Q6); A7 no gameplay value changes; A8 a capture compares before and after in the same fight and is sent to the owner; A9 color-code and the weapon nodes state the new impact look.
- gdd_check: Read color-code, readability-rules, shotgun, task-464-player-shot-colour (open, confirmed, todo) and its subtasks 01 and 02. Placement: the impact look is a separate owner point from shot and flash colour, so a new global task, not a replan of task-464; task-464 already recolours today's impact sparks green (A1, A2) and is left unchanged; task-465 runs after task-464 and replaces those sparks with the new burst, reusing its green (Q1). Colours: projectile = shotgun pellets (shotgun node: projectile pellets, white-gold); laser = burst rifle and turrets (task-464 decision: turrets are laser in look, projectile in the rules, rule versus rendering). Reference: white and yellow-orange spikes with an orange glow, so "blanc" for projectiles is read as white dominant with the warm accents, consistent with the white-gold bullet look (Q2). The reference burst is large; player shots hit very often, so size and life are bounded (Q3, readability rule: Shooter spotted in under a second). Black triangles are kept as in the reference (owner image) unless the owner says otherwise (Q2). No conflict with readability-rules (green is the task-464 player laser green, not the Shooter acid green). Edit on confirmation: one new color-code row for player shot impacts. Image stored as keep in Art/Source/impact-burst/owner-ref-impact-burst-2026-11-05.png (Q7). In-progress task-462 (extractor blast) unrelated. No open value needed. No code read.
- confirmed_by_owner: yes (2026-11-06, "prépare la tâche sans lancer, j'accepte les propostions.")
- milestone: M2
- next_step: task-465-03-impact-expand
- started: 2026-11-06T09:25:46Z
- waiting_for: none
- executor_tokens: input 194, cache write 214809, cache read 12709509, output 61366
- executor_usage: runs 1, turns 95, context start 18k peak 227k, resumes 1, 18 min, units 1.85 M
- planning_tokens: input 22, cache write 39693, cache read 359859, output 12653
- planning_usage: aedb5b0d "Plan player shot impact FX" 1/1 (tasks 465), units 0.15 M
- result: open

## Questions
Decided 2026-11-06 (owner "prépare la tâche sans lancer, j'accepte les propostions."): every default below is accepted; values provisional, owner 2026-11-06.
- Q1 Les impacts sont une nouvelle tâche (task-465), lancée après task-464 qui reste telle quelle (elle colore en vert les étincelles actuelles) ; task-465 remplace ensuite ces étincelles par la nouvelle explosion. Default: oui, tâche séparée après task-464.
- Q2 Couleurs : "projectile" = plombs du fusil à pompe, impact blanc dominant en gardant les accents or-orange et le halo chaud de ta référence ; "laser" = fusil laser et tourelles, impact vert (cœur blanc-vert pâle, pointes vert vif, halo vert foncé) ; les petits triangles noirs et blancs de la référence sont gardés tels quels. Default: oui.
- Q3 Taille et durée : ta référence est une grosse explosion, or les tirs touchent très souvent. L'impact reste petit et bref : environ la taille des étincelles actuelles (au plus le double), disparu en une fraction de seconde, pour que robots et chiffres de dégâts restent lisibles. Default: oui.
- Q4 Tourelles : même impact vert que le fusil laser, à la taille des tirs de tourelle. Default: oui.
- Q5 L'impact apparaît partout où les étincelles apparaissent aujourd'hui (robot, roche, baril), sans variante plus grosse pour les coups critiques. Default: oui.
- Q6 Impacts des tirs ennemis, laser de minage et laser de visée inchangés. Default: oui.
- Q7 Ton image est gardée comme référence de style ("keep") dans Art/Source/impact-burst/owner-ref-impact-burst-2026-11-05.png et citée dans le code couleur. Default: oui, keep.

## Subtasks
1. task-465-01-laser-impact-slice (vault/reference/tasks/task-465-shot-impact-burst/task-465-01-laser-impact-slice.md) — bp: Burst rifle impacts become the green flat cartoon burst of the owner reference (representative slice)
2. task-465-02-slice-capture (vault/reference/tasks/task-465-shot-impact-burst/task-465-02-slice-capture.md) — capture: Capture the burst rifle impact slice in a fight and send it to the owner before expansion
3. task-465-03-impact-expand (vault/reference/tasks/task-465-shot-impact-burst/task-465-03-impact-expand.md) — bp: Turret impacts get the green burst sized to the turret shot; shotgun pellet impacts get the same burst in white with warm gold-orange accents
4. task-465-04-doc (vault/reference/tasks/task-465-shot-impact-burst/task-465-04-doc.md) — doc: Update the bp-* and sys-* nodes of the impact effects and check color-code, burst-rifle, shotgun and turret match the result
5. task-465-05-capture (vault/reference/tasks/task-465-shot-impact-burst/task-465-05-capture.md) — capture: Before/after capture of a fight with burst rifle, turret and shotgun impacts, enemy shots and the Shooter present, sent to the owner

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-465-01-laser-impact-slice]] — step 1, bp.
- [[task-465-02-slice-capture]] — step 2, capture.
- [[task-465-03-impact-expand]] — step 3, bp.
- [[task-465-04-doc]] — step 4, doc.
- [[task-465-05-capture]] — step 5, capture.
