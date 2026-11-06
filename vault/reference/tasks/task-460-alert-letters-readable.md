---
id: task-460-alert-letters-readable
description: "Global task: alert bar letters C, B, S stay readable when a horde \"!\" sits on or moves over them."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Alert bar letters never hidden by a horde mark

- request: Owner 2026-11-05 (verbatim, whole message): "* Rajouter 1 sec lorsqu'on level up ( laisser le temps de voir les effets du level up ) * Voilà le rendu actuel de l'UI ( voir image 1 ): * Le problème avec le top où on voit les events, c'est que le point d'exclamation du swarm cache le "C" de l'event commande ( et le "B" Bonus quand le point d'exclamation bouge ) * Toute la partie en bas de l'écran n'est pas intégré comme sur l'image 2 * Sur l'image 3, l'image de vent au sol ne fonctionne pas du tout, essaye de trouver des références * Les particules d'explosion fonctionne super bien ! J'adore le style cartoon pour les particules !" Images kept (owner images): image 1 Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-05b.webp (current HUD: two red "!" on the alert bar, only B and S readable, the C hidden under a "!"; health bar, four separately framed slots linked by small connectors and a bare "Lv 1" XP bar, all floating apart), image 2 Art/Source/hud-unified/reference/owner-hud-target-2026-11-05.webp (target: health bar and the four slots inside one riveted metal panel, XP bar in its own riveted frame along the bottom edge with "Lv 1" in a framed cap; C, B, S readable, "!" not covering them; it matches the retained mockup Art/Source/hud-unified/20261105-152627-hud-unified-1.png), image 3 Art/Source/extractor-fx/reference/owner-ground-blast-2026-11-05.webp (the landed extractor at level start; the ground blast shows as large blurry white oval blobs on the floor). The last point is praise, recorded as a style preference in art-style-guide A10, no task. This task is the point "le point d'exclamation du swarm cache le "C" de l'event commande ( et le "B" Bonus quand le point d'exclamation bouge )".
- goal: Every event letter on the alert bar can always be read.
- acceptance: A1 in game, a "!" horde mark at or near the position of C, B or S never hides any part of the letter, at level start (the case of Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-05b.webp) and while the marks move with kills; A2 per Q36: the letter is drawn in front, the "!" rises above the bar top as in Art/Source/hud-unified/reference/owner-hud-target-2026-11-05.webp and the retained mockup Art/Source/hud-unified/20261105-152627-hud-unified-1.png; letters and marks keep their positions on the bar (they mean timings); A3 marks and letters stay inside the screen at the four umg-layout resolutions; A4 sprites of task-453, colours, fill and the horde rule of task-454 unchanged; A5 widget follows umg-layout and starts from the assets as they are: an uncommitted owner edit is committed alone first; compiles with no runtime error in PIE; A6 No capture and no stress run in this task unless the owner asks: he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it. The report also gives the token cost of this task, per subtask and in total, from cost-log (units, turns, runs, resumes).
- gdd_check: Read level-flow (hordes as icons on the alert bar, console at 40 %), art-prompts-hud-unified-mockup (453: "!" and C, B, S set on the track). No conflict; level-flow edited at confirmation. Moving a letter or a mark sideways would misplace a timing, hence the default of Q36.
- confirmed_by_owner: no (plan and questions handed back 2026-11-06)
- milestone: M2
- next_step: task-460-01-alert-bar
- started: open
- waiting_for: owner answers to Q35 to Q38 and his go
- result: open

## Questions
Asked 2026-11-06, each with a default; open until the owner answers.
- Q36 Barre d'alerte : Default: comme sur l'image 2, le « ! » dépasse au-dessus du haut de la barre et passe derrière la lettre ; la lettre reste entière, devant, lisible ; rien ne bouge de sa position sur la barre (les positions sont des moments). Autre choix : la lettre ou le « ! » se décale de côté quand ils se chevauchent.

## Subtasks
1. task-460-01-alert-bar (vault/reference/tasks/task-460-alert-letters-readable/task-460-01-alert-bar.md) — bp: The alert bar draws event letters whole in front of horde marks, the marks rising above the bar top.
2. task-460-02-doc (vault/reference/tasks/task-460-alert-letters-readable/task-460-02-doc.md) — doc: Record the letter and mark layering.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-460-01-alert-bar]] — step 1, bp.
- [[task-460-02-doc]] — step 2, doc.
