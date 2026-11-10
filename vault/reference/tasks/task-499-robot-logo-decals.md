---
id: task-499-robot-logo-decals
description: "Global task: The yellow Kessler-Voss emblem added as a decal on the Heavy, Kamikaze and Chaser v2 meshes, like the shield emblem, without regenerating the Meshy meshes."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Yellow Kessler-Voss emblem decal on Heavy, Kamikaze, Chaser v2

- request: Context: read-only Code Architect scan of the GDD against the code (2026-11-10); the owner asked for questions on the top 4 gaps, answered them, then said: "lance les 4 tâches, je ne sais pas où sont génerés les json, il s'agit d'un script cpp qui s'en occupe". Point 4 (yellow logo plate on robots), owner decision: "Décalque sur le mesh, 3 robots": a yellow decal like the shield emblem, on Heavy, Kamikaze and Chaser v2, without regenerating the Meshy meshes/textures. The plate passes no pillar test strictly; the owner chose to keep it. Owner answer 2026-11-10 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q1, Q2, Q3, Q4 accepted.
- goal: Heavy, Kamikaze and Chaser v2 (and the shield chaser v2 that shares its body) carry the yellow Kessler-Voss emblem as a decal on the mesh, readable from the game camera, like the shield emblem and the chaser v1 hood emblem.
- acceptance: A1 Each of Heavy, Kamikaze, Chaser v2 and shield chaser v2 shows the yellow Kessler-Voss emblem on its top surface, readable from the game camera. A2 The emblem stays fixed on its part while the robot walks, attacks, telegraphs and on its death pieces. A3 Meshy meshes and their base textures are not regenerated; v1 assets untouched. A4 Colours as color-code and kessler-voss-logo: company yellow, never orange; the kamikaze core and the heavy stripes stay readable. A5 Style-guide critic verdict on the capture frames. A6 Before and after -game capture sent to the owner.
- gdd_check: Read color-code (all robots: red eyes + yellow logo plate), kessler-voss-logo (emblem variation 4 for small sizes; never orange), art-prompts-robots-meshy (kamikaze and chaser v2 sheets show no plate), enemy-heavy, enemy-kamikaze, enemy-chaser (v1 has the company emblem on the hood top; v2 is company yellow, so yellow on yellow). Conflicts: the Meshy sheets show no plate for kamikaze and chaser v2, the owner adds it as a decal; pillar tests not passed strictly, owner keeps it (recorded in kessler-voss-logo). No open value. Edits: color-code, kessler-voss-logo.
- verification_level: standard
- confirmed_by_owner: yes (2026-11-10, "ok")
- milestone: M2
- next_step: task-499-07-doc
- started: 2026-11-10T20:42:05Z
- waiting_for: none
- result: open

## Questions
Decided 2026-11-10 (owner "ok"): every default below is accepted; values provisional, owner 2026-11-10.
- Q1 Emplacement : sur la surface du dessus vue par la caméra du jeu (heavy : dessus du bloc tête-torse ; kamikaze : dessus de la tête, pas sur le noyau orange ; chaser v2 : dessus du capot comme le chaser v1). Défaut : oui.
- Q2 Motif : l'emblème seul (KV et facettes, parties jaunes, comme sur le bouclier) posé direct sur le corps du heavy et du kamikaze ; le chaser v2 étant jaune, même traitement que l'emblème du capot du chaser v1 pour qu'il se lise sur le jaune. Défaut : oui.
- Q3 Taille : environ un tiers de la largeur de la surface du dessus, lisible à la caméra du jeu sans couvrir les yeux ni les rayures. Défaut : oui.
- Q4 Le shield chaser v2 (même corps que le chaser v2) reçoit aussi l'emblème. Défaut : oui.

## Subtasks
1. task-499-01-emblem-material (vault/reference/tasks/task-499-robot-logo-decals/task-499-01-emblem-material.md) — art: The emblem decal source and material shared by the three robots, from the existing emblem cutout, as on the shield.
2. task-499-02-heavy-decal (vault/reference/tasks/task-499-robot-logo-decals/task-499-02-heavy-decal.md) — bp: Emblem placed on the Heavy.
3. task-499-03-kamikaze-decal (vault/reference/tasks/task-499-robot-logo-decals/task-499-03-kamikaze-decal.md) — bp: Emblem placed on the Kamikaze.
4. task-499-04-chaser-v2-decal (vault/reference/tasks/task-499-robot-logo-decals/task-499-04-chaser-v2-decal.md) — bp: Emblem placed on the Chaser v2, carried by the shield chaser v2.
5. task-499-05-capture (vault/reference/tasks/task-499-robot-logo-decals/task-499-05-capture.md) — capture: A -game capture of the three robots with the emblem, beside frames before.
6. task-499-06-style-review (vault/reference/tasks/task-499-robot-logo-decals/task-499-06-style-review.md) — style-guide: Critic verdict on the capture frames against the art style guide, colour code and logo rules.
7. task-499-07-doc (vault/reference/tasks/task-499-robot-logo-decals/task-499-07-doc.md) — doc: Document the emblem decal: asset, placement per robot, how it follows the animation.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-499-01-emblem-material]] — step 1, art.
- [[task-499-02-heavy-decal]] — step 2, bp.
- [[task-499-03-kamikaze-decal]] — step 3, bp.
- [[task-499-04-chaser-v2-decal]] — step 4, bp.
- [[task-499-05-capture]] — step 5, capture.
- [[task-499-06-style-review]] — step 6, style-guide.
- [[task-499-07-doc]] — step 7, doc.
