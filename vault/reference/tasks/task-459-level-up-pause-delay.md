---
id: task-459-level-up-pause-delay
description: "Global task: The upgrade cards open 1 s later after a level-up, so the level-up effect can be seen."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# One more second before the level-up cards

- request: Owner 2026-11-05 (verbatim, whole message): "* Rajouter 1 sec lorsqu'on level up ( laisser le temps de voir les effets du level up ) * Voilà le rendu actuel de l'UI ( voir image 1 ): * Le problème avec le top où on voit les events, c'est que le point d'exclamation du swarm cache le "C" de l'event commande ( et le "B" Bonus quand le point d'exclamation bouge ) * Toute la partie en bas de l'écran n'est pas intégré comme sur l'image 2 * Sur l'image 3, l'image de vent au sol ne fonctionne pas du tout, essaye de trouver des références * Les particules d'explosion fonctionne super bien ! J'adore le style cartoon pour les particules !" Images kept (owner images): image 1 Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-05b.webp (current HUD: two red "!" on the alert bar, only B and S readable, the C hidden under a "!"; health bar, four separately framed slots linked by small connectors and a bare "Lv 1" XP bar, all floating apart), image 2 Art/Source/hud-unified/reference/owner-hud-target-2026-11-05.webp (target: health bar and the four slots inside one riveted metal panel, XP bar in its own riveted frame along the bottom edge with "Lv 1" in a framed cap; C, B, S readable, "!" not covering them; it matches the retained mockup Art/Source/hud-unified/20261105-152627-hud-unified-1.png), image 3 Art/Source/extractor-fx/reference/owner-ground-blast-2026-11-05.webp (the landed extractor at level start; the ground blast shows as large blurry white oval blobs on the floor). The last point is praise, recorded as a style preference in art-style-guide A10, no task. This task is the point "Rajouter 1 sec lorsqu'on level up ( laisser le temps de voir les effets du level up )".
- goal: After a level-up the player sees the gold text and the floor wave play before the cards cover the screen.
- acceptance: A1 in game, the 3-card screen opens about 1.8 s after the level-up instead of about 0.8 s, read in a PIE run or in a test, not from a constant alone; A2 the game keeps running during that time as today (Q35); A3 several levels gained at once wait once, then chain their screens as today; A4 the gold text, the floor wave, the push, the heal and a bonus chest choice screen are unchanged; A5 builds; existing tests pass, only expectations this change breaks are updated; A6 No capture and no stress run in this task unless the owner asks: he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it. The report also gives the token cost of this task, per subtask and in total, from cost-log (units, turns, runs, resumes).
- gdd_check: Read xp-level-up (level-up sequence: about 0.8 s then pause and cards; level-up effect of task-441 and 449). No conflict. "1 sec" read as +1 s on the existing 0.8 s, hence 1.8 s; xp-level-up edited at confirmation. Whether the game runs or freezes during the wait: Q35.
- confirmed_by_owner: no (plan and questions handed back 2026-11-06)
- milestone: M2
- next_step: task-459-01-delay
- started: open
- waiting_for: owner answers to Q35 to Q38 and his go
- result: open

## Questions
Asked 2026-11-06, each with a default; open until the owner answers.
- Q35 « Rajouter 1 sec » : Default: l'écran des 3 cartes s'ouvre environ 1,8 s après le level up au lieu de 0,8 s ; pendant ce temps le jeu continue comme aujourd'hui (robots, tirs, ramassage). Plusieurs niveaux d'un coup : une seule attente avant le premier écran, puis les écrans s'enchaînent comme aujourd'hui. Autre choix : le jeu se fige (ou ralentit) pendant cette seconde.

## Subtasks
1. task-459-01-delay (vault/reference/tasks/task-459-level-up-pause-delay/task-459-01-delay.md) — cpp: The card screen opens about 1.8 s after a level-up; nothing else in the sequence changes.
2. task-459-02-doc (vault/reference/tasks/task-459-level-up-pause-delay/task-459-02-doc.md) — doc: Record the new delay.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-459-01-delay]] — step 1, cpp.
- [[task-459-02-doc]] — step 2, doc.
