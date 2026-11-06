---
id: task-453-alert-bar-redesign
description: "Global task: The alert bar with its swarm, console, bonus and shuttle marks is redrawn in the retained unified look; its rules stay."
parent: tasks-done-index
kind: task
status: done
---
# Alert bar redesigned and better integrated

- request: Owner 2026-11-05 (verbatim): "Il faudrait retravailler les différents UI in game , j'aimerais que la vie se trouve juste au dessus des armes. Owner answer 2026-11-05 (relayed by the main session, verbatim): "Q33 - Pas dedans, il faudrait que ce qui enveloppe le radar devra englober le nombre de minerai. ( voir ex en image ) Je répéte, c'est un exemple, pas à reprendre tel quel. Ok pour le reste ( les valeurs par défaut )" = the plan and the defaults of Q28 accepted.

Il faudrait refaire la barre d'évolution ( où on voit le swarm qui arrive et le C ( pour commande ) et  B pour bonus ), que ça soit mieux intégré

Il faudrait que le premier swarm soit fixe et que le second ne bouge pas avant que le premier ne soit finit. ( donc tant que le premier swarm n'est pas fini, les swarm ne réagissent pas au nombre de kill )

La barre d'xp, il faudrait une séparation légére par 5 et lorsqu'on gagne de l'xp, il faudrait un effet fantome de l'xp ( un peu comme quand on perds des pv, mais à l'inverse ).

Il faudrait aussi quelque chose pour entouré le radar qu'il fasse mieux intégré ( qu'un simple cercle ). Peut être que ce morceau d'interface inclut le nombre de minerai ( plutot que de flotter juste à coté ).

Le but va d'être faire une interface unifiée plutôt que chaque des petits morceaux d'interface séparée, j'attends des propositions visuelles de ta part en utilisant l'api chatgpt." Clarified the same day (relayed): "l'api chatgpt" is the Kie.ai key (KIE_API_KEY) with its GPT image model, i.e. Tools/art/kie_image.js. His screenshot of the current HUD: Art/Source/hud-unified/reference/owner-hud-current-2026-11-05.webp (radar top left with the ore count floating at its right, health bar under the radar, alert bar top centre with two red swarm marks and C, B, S letters, 4 weapon slots bottom centre, XP bar along the bottom edge with "Lv 1"). This task is the point "refaire la barre d'évolution ( où on voit le swarm qui arrive et le C ( pour commande ) et B pour bonus ), que ça soit mieux intégré".
- goal: The alert bar reads as part of the unified HUD, marks included.
- acceptance: A0 Starts only once task-451-hud-mockups is done: the look comes from the mockup the owner retained there (recorded in its art-prompts node, files under Art/Source/hud-unified/). A1 the alert bar, its fill and its marks (swarms, console, bonus chest, shuttle) follow the retained mockup, per Q28; A2 marks keep their positions, done states and colours by color-code; the rules (fill, console pause, icon positions) are unchanged by this task; A3 Widgets follow umg-layout (root Canvas, anchors, no fixed pixel sizes) and start from the assets as they are: an uncommitted owner edit is committed alone first. A4 compiles and runs with no error, existing tests pass, only expectations this change breaks are updated; A5 No capture run by the capture agent unless the owner asks (Q26): he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it. The report also gives the token cost of this task, per subtask and in total, from cost-log.
- gdd_check: Read level-flow, difficulty, sys-hud-pressure. Look change only (rules versus rendering); the swarm rule is task-454.
- confirmed_by_owner: yes (2026-11-05, "Q33 - Pas dedans, il faudrait que ce qui enveloppe le radar devra englober le nombre de minerai. ( voir ex en image ) Je répéte, c'est un exemple, pas à reprendre tel quel. Ok pour le reste ( les valeurs par défaut )")
- milestone: M2
- next_step: none
- started: 2026-11-06T00:46:45Z
- waiting_for: none
- result: Done. Commit 851779a5. wbp-alert-objectives updated: components (AlertBar dark track, AlertFrame, C/B/S font 28, horde marks as T_HUD_AlertMark images), layout table (anchors 0.5,0, alignment, offsets, size mode, z per widget), PlaceAlertIcon Widget param, HideForOffer/RestoreAfterOffer/HideForEnd frame handling, assets, verified, relation to art-prompts-hud-unified-mockup (both ends); blueprints-ui-index gloss. Lint clean (pre-existing warnings only).

## Questions
Decided 2026-11-05 (owner "Q33 - Pas dedans, il faudrait que ce qui enveloppe le radar devra englober le nombre de minerai. ( voir ex en image ) Je répéte, c'est un exemple, pas à reprendre tel quel. Ok pour le reste ( les valeurs par défaut )"): every default below is accepted; values provisional, owner 2026-11-05.
- Q28 Barre d'alerte. Default: elle reste en haut au centre ; son cadre, son remplissage et ses marqueurs prennent le look de la maquette retenue ; si la maquette montre des icônes à la place des lettres C, B, S, ce sont des icônes (couleurs du code couleur) ; aucune règle ne change ici.

## Subtasks
1. task-453-01-art (vault/reference/tasks/task-453-alert-bar-redesign/task-453-01-art.md) — art: Cut the alert bar frame and mark sprites of the retained mockup.
2. task-453-02-alert-bar (vault/reference/tasks/task-453-alert-bar-redesign/task-453-02-alert-bar.md) — bp: The alert bar widget takes the retained look, marks driven as before.
3. task-453-03-doc (vault/reference/tasks/task-453-alert-bar-redesign/task-453-03-doc.md) — doc: Docs describe the new look.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-453-01-art]] — step 1, art.
- [[task-453-02-alert-bar]] — step 2, bp.
- [[task-453-03-doc]] — step 3, doc.
