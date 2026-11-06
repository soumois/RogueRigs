---
id: task-472-actor-blob-shadows
description: "Global task: Each listed actor gets a soft purple-grey shadow directly beneath it that follows it and ignores every light, with adjustable opacity and scale; their light shadows are turned off."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Light-independent purple-grey shadows under actors

- request: Owner 2026-11-05 (verbatim): "J'aimerais faire des changements sur le design (donc ça va aller à l'encontre de ce qu'il y a dans le vault). - On va passer plus en cartoon * (image 1 au sol) change la texture du sol * Changer le système d'ombre (voilà ce qu'on va tester) : --- Each shadow must: * Be positioned directly beneath its associated actor. * Follow the actor's world position. * Remain independent of directional lights and other light sources. * Use a dark desaturated purple-grey colour, never pure black. * Have adjustable opacity and scale. * Have slightly softened edges, while retaining a clearly readable silhouette. * Avoid excessive transparency or overly blurred edges. --- * Essaye de calibrer la lumière comme la lumière du joueur sur l'image * Essaye de ré-obtenir le rendu sur l'image concernant les murs Ne lance aucune tâche". Image 1 kept (owner image): Art/Source/floor-texture/owner-2026-11-05-floor-cartoon.webp (seamless hand-painted cartoon magenta-purple flagstones, dark cracks with a pink rim highlight). Image 2 kept (owner image): Art/Source/cartoon-look/reference/owner-2026-11-05-ingame-light-walls.webp (in-game view: warm orange-pink light pool around the miner on the purple floor; raised purple plateaus with light-lilac tops, a glowing orange rim along the cliff lip, dark stratified cliff faces, pink crystals). The request is split into four global tasks (one per owner point): task-470 floor, task-471 walls, task-472 actor shadows, task-473 miner light. "Ne lance aucune tâche": planned only. This task: "Changer le système d'ombre (voilà ce qu'on va tester)" with the seven "Each shadow must" points.
- goal: Every listed actor has a readable purple-grey shadow under it that follows it and does not depend on any light, tunable in the editor.
- acceptance: A1 per Q1 the miner, the drone, every robot type, the turrets, the barrels and the bonus chest each have one shadow directly beneath them on the floor; A2 the shadow follows the actor's world position every frame (moving, knocked back, dashing) and disappears with the actor; the drone shadow stays on the floor under it; A3 the shadow does not change with the sun, the sky, the miner light or any other light (same look in the miner light pool and far from it); A4 colour dark desaturated purple-grey (start value per Q3), never pure black; A5 opacity and scale are adjustable in the editor per actor type and by one global setting for each, without code change; A6 slightly softened edge with a clearly readable silhouette: not very transparent, not very blurred; A7 per Q2 these actors cast no light shadow any more and the task-264 blob discs are replaced; wall shadows and the sun stay unchanged; A8 no new frame-time cost visible with a full wave of robots (game-thread time unchanged within noise); A9 compiles, existing tests pass (only expectations broken by this change are updated), no new runtime error in PIE; A10 a -game capture compared with the look before (Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-06.webp) is sent to the owner; the report gives the token cost per subtask and in total.
- gdd_check: Read visual-style (miner light casts only rock, ore, heal ore shadows, task-445; sun and sky unchanged), art-style-guide A12 (strong directional light with lifted shadows), handover-2026-10-21-lighting-floor (task-264 blob shadows, owner decision 2026-10-21 "blob disc only, no sun shadow" for robots, miner and drone, then task-275 put sun shadows back on). Conflicts: A12 and visual-style gain the actor shadow rule; edits in the spec apply on confirmation. Not in the GDD, decided by questions: which actors (Q1), what happens to their light shadows and the task-264 discs (Q2), start values (Q3). No code read.
- confirmed_by_owner: no (plan and questions handed back 2026-11-06)
- milestone: M2
- next_step: task-472-01-shadow-component
- started: open
- waiting_for: Owner answers to Q1-Q3 (actors, current shadows, start values).
- result: open

## Questions
Asked 2026-11-06, each with a default; open until the owner answers.
- Q1 Acteurs : Default: mineur, drone, tous les robots (chaser, chaser à bouclier, kamikaze, shooter, heavy), tourelles, barils, coffre bonus. Pas d'ombre pour les ramassables, les tirs, les blocs de terrain, la console, la navette et l'extracteur.
- Q2 Ombres actuelles : Default: ces acteurs ne projettent plus aucune ombre de lumière (soleil, ciel, lumière du mineur) et les disques d'ombre de la tâche 264 sont remplacés par le nouveau système ; les murs gardent leurs ombres et le soleil ne change pas.
- Q3 Valeurs de départ (provisoires, réglables par toi dans l'éditeur) : Default: violet-gris sombre désaturé, environ sRGB (45, 35, 55), jamais noir ; opacité 0,6 ; diamètre 1,2 x l'emprise de l'acteur ; bord adouci sur les 20 % extérieurs du rayon ; un réglage par type d'acteur plus un multiplicateur global d'opacité et d'échelle ; le drone garde une ombre de taille fixe quelle que soit sa hauteur.

## Subtasks
1. task-472-01-shadow-component (vault/reference/tasks/task-472-actor-blob-shadows/task-472-01-shadow-component.md) — cpp: One generic shadow component placed under its actor, following it, with per-type and global opacity and scale, and the listed actors stop casting light shadows.
2. task-472-02-shadow-material-wiring (vault/reference/tasks/task-472-actor-blob-shadows/task-472-02-shadow-material-wiring.md) — bp: The purple-grey soft-edged shadow material, assigned to every listed actor with its start values.
3. task-472-03-capture (vault/reference/tasks/task-472-actor-blob-shadows/task-472-03-capture.md) — capture: Check the shadows in game against the seven owner points.
4. task-472-04-doc (vault/reference/tasks/task-472-actor-blob-shadows/task-472-04-doc.md) — doc: Docs describe the shadow component, its settings and the material.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-472-01-shadow-component]] — step 1, cpp.
- [[task-472-02-shadow-material-wiring]] — step 2, bp.
- [[task-472-03-capture]] — step 3, capture.
- [[task-472-04-doc]] — step 4, doc.
