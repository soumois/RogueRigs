---
id: task-462-extractor-blast-cartoon
description: "Global task: The extractor ground blast, today blurry white blobs, is redone from found references in the cartoon style of the explosions."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Extractor ground blast redone in cartoon style from references

- request: Owner 2026-11-05 (verbatim, whole message): "* Rajouter 1 sec lorsqu'on level up ( laisser le temps de voir les effets du level up ) * Voilà le rendu actuel de l'UI ( voir image 1 ): * Le problème avec le top où on voit les events, c'est que le point d'exclamation du swarm cache le "C" de l'event commande ( et le "B" Bonus quand le point d'exclamation bouge ) * Toute la partie en bas de l'écran n'est pas intégré comme sur l'image 2 * Sur l'image 3, l'image de vent au sol ne fonctionne pas du tout, essaye de trouver des références * Les particules d'explosion fonctionne super bien ! J'adore le style cartoon pour les particules !" Images kept (owner images): image 1 Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-05b.webp (current HUD: two red "!" on the alert bar, only B and S readable, the C hidden under a "!"; health bar, four separately framed slots linked by small connectors and a bare "Lv 1" XP bar, all floating apart), image 2 Art/Source/hud-unified/reference/owner-hud-target-2026-11-05.webp (target: health bar and the four slots inside one riveted metal panel, XP bar in its own riveted frame along the bottom edge with "Lv 1" in a framed cap; C, B, S readable, "!" not covering them; it matches the retained mockup Art/Source/hud-unified/20261105-152627-hud-unified-1.png), image 3 Art/Source/extractor-fx/reference/owner-ground-blast-2026-11-05.webp (the landed extractor at level start; the ground blast shows as large blurry white oval blobs on the floor). The last point is praise, recorded as a style preference in art-style-guide A10, no task. This task is the point "Sur l'image 3, l'image de vent au sol ne fonctionne pas du tout, essaye de trouver des références", with the praise "J'adore le style cartoon pour les particules" as the style direction. Owner answer 2026-11-06 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q38 accepted.
- goal: The rotor blast on the floor reads as wind pushed outward, in the cartoon style the owner likes.
- acceptance: A1 3 to 5 references of stylised ground wind or rotor downwash found and recorded in a vault node (link, what to take from each), none copied into the repo, and listed in the final report (Q38); A2 in game under the landed extractor, the ground blast no longer shows the large blurry white ovals of Art/Source/extractor-fx/reference/owner-ground-blast-2026-11-05.webp: sharp-edged cartoon shapes and curved wind strokes pushed outward along the floor, close to the chosen reference and to art-style-guide A10; A3 strength still falls with the base height, full when landed, none at 400 uu, and it follows both takeoffs as today; A4 the rotor swirls under the rotors (task-442), rotor spin, shadows and timings unchanged; no gameplay effect, no Event Tick; Extraction tests pass; A5 No capture and no stress run in this task unless the owner asks: he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it. The report also gives the token cost of this task, per subtask and in total, from cost-log (units, turns, runs, resumes).
- gdd_check: Read extraction (ground blast of task-443: outward dust, height fade, pale grey-beige, low opacity), art-style-guide (A10 cartoon explosions), task-443. Possible conflict: task-442 set "no thin lines" for the rotor swirls; cartoon wind usually uses strokes; default of Q38 keeps that rule on the swirls and allows strokes on the ground blast only. extraction and art-style-guide A10 edited at confirmation. Owner image kept: Art/Source/extractor-fx/reference/owner-ground-blast-2026-11-05.webp.
- confirmed_by_owner: yes (2026-11-06, "ok")
- milestone: M2
- next_step: task-462-03-doc
- started: 2026-11-06T04:40:28Z
- waiting_for: none
- result: open

## Questions
Decided 2026-11-06 (owner "ok"): every default below is accepted; values provisional, owner 2026-11-06.
- Q38 Souffle au sol : Default: l'Executor cherche 3 à 5 références de vent ou de souffle de rotor au sol en style cartoon, les note dans le vault (lien et ce qu'on en retient, aucune image copiée), te les liste dans le rapport, et refait l'effet d'après la plus proche du style de tes explosions : formes nettes à bords francs, traits de vent courbés qui partent vers l'extérieur, plus de taches floues ; seul le souffle au sol change, les volutes sous les hélices restent. Autre choix : il s'arrête après les références et attend que tu en choisisses une.

## Subtasks
1. task-462-01-references (vault/reference/tasks/task-462-extractor-blast-cartoon/task-462-01-references.md) — art: Find and record 3 to 5 cartoon ground wind references and the one the effect follows.
2. task-462-02-ground-blast (vault/reference/tasks/task-462-extractor-blast-cartoon/task-462-02-ground-blast.md) — bp: The ground blast becomes the cartoon wind of the chosen reference, height fade kept.
3. task-462-03-doc (vault/reference/tasks/task-462-extractor-blast-cartoon/task-462-03-doc.md) — doc: Docs describe the new ground blast and its references.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-462-01-references]] — step 1, art.
- [[task-462-02-ground-blast]] — step 2, bp.
- [[task-462-03-doc]] — step 3, doc.
