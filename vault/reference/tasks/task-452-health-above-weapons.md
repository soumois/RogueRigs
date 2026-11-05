---
id: task-452-health-above-weapons
description: "Global task: The health bar leaves the top left and sits just above the weapon slots, in the retained unified look."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Health bar just above the weapon slots

- request: Owner 2026-11-05 (verbatim): "Il faudrait retravailler les différents UI in game , j'aimerais que la vie se trouve juste au dessus des armes. Owner answer 2026-11-05 (relayed by the main session, verbatim): "Q33 - Pas dedans, il faudrait que ce qui enveloppe le radar devra englober le nombre de minerai. ( voir ex en image ) Je répéte, c'est un exemple, pas à reprendre tel quel. Ok pour le reste ( les valeurs par défaut )" = the plan and the defaults of Q27 accepted.

Il faudrait refaire la barre d'évolution ( où on voit le swarm qui arrive et le C ( pour commande ) et  B pour bonus ), que ça soit mieux intégré

Il faudrait que le premier swarm soit fixe et que le second ne bouge pas avant que le premier ne soit finit. ( donc tant que le premier swarm n'est pas fini, les swarm ne réagissent pas au nombre de kill )

La barre d'xp, il faudrait une séparation légére par 5 et lorsqu'on gagne de l'xp, il faudrait un effet fantome de l'xp ( un peu comme quand on perds des pv, mais à l'inverse ).

Il faudrait aussi quelque chose pour entouré le radar qu'il fasse mieux intégré ( qu'un simple cercle ). Peut être que ce morceau d'interface inclut le nombre de minerai ( plutot que de flotter juste à coté ).

Le but va d'être faire une interface unifiée plutôt que chaque des petits morceaux d'interface séparée, j'attends des propositions visuelles de ta part en utilisant l'api chatgpt." Clarified the same day (relayed): "l'api chatgpt" is the Kie.ai key (KIE_API_KEY) with its GPT image model, i.e. Tools/art/kie_image.js. His screenshot of the current HUD: Art/Source/hud-unified/reference/owner-hud-current-2026-11-05.webp (radar top left with the ore count floating at its right, health bar under the radar, alert bar top centre with two red swarm marks and C, B, S letters, 4 weapon slots bottom centre, XP bar along the bottom edge with "Lv 1"). This task is the point "j'aimerais que la vie se trouve juste au dessus des armes".
- goal: In game the health bar is read just above the weapons, in the unified HUD look.
- acceptance: A0 Starts only once task-451-hud-mockups is done: the look comes from the mockup the owner retained there (recorded in its art-prompts node, files under Art/Source/hud-unified/). A1 the health bar and its "current / max" text sit just above the 4 weapon slots, centred on them, per Q27, and no longer under the radar; A2 its look follows the retained mockup; A3 drain, damage chunk, hit flash, shake, punch and the green/yellow/red colour still work; the red damage vignette and GAME OVER are unchanged; A4 the layout holds at other resolutions and ratios per umg-layout, Widgets follow umg-layout (root Canvas, anchors, no fixed pixel sizes) and start from the assets as they are: an uncommitted owner edit is committed alone first. A5 compiles and runs with no error, existing tests pass, only expectations this change breaks are updated; A6 No capture run by the capture agent unless the owner asks (Q26): he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it. The report also gives the token cost of this task, per subtask and in total, from cost-log.
- gdd_check: Read sys-hud-health-bar (effects to preserve), umg-layout. No gdd node places the health bar; nothing conflicts.
- confirmed_by_owner: yes (2026-11-05, "Q33 - Pas dedans, il faudrait que ce qui enveloppe le radar devra englober le nombre de minerai. ( voir ex en image ) Je répéte, c'est un exemple, pas à reprendre tel quel. Ok pour le reste ( les valeurs par défaut )")
- milestone: M2
- next_step: task-452-01-art
- started: 2026-11-05T23:43:51Z
- waiting_for: none
- result: open

## Questions
Decided 2026-11-05 (owner "Q33 - Pas dedans, il faudrait que ce qui enveloppe le radar devra englober le nombre de minerai. ( voir ex en image ) Je répéte, c'est un exemple, pas à reprendre tel quel. Ok pour le reste ( les valeurs par défaut )"): every default below is accepted; values provisional, owner 2026-11-05.
- Q27 Vie. Default: barre centrée juste au-dessus des 4 emplacements d'armes, de leur largeur totale, texte « 100 / 100 » dedans ; tous ses effets (glissement, morceau clair, flash, secousse, couleur vert/jaune/rouge) restent ; la vignette rouge et le GAME OVER ne bougent pas.

## Subtasks
1. task-452-01-art (vault/reference/tasks/task-452-health-above-weapons/task-452-01-art.md) — art: Cut the health bar pieces of the retained mockup as UI sprites, or report that none is needed.
2. task-452-02-health-panel (vault/reference/tasks/task-452-health-above-weapons/task-452-02-health-panel.md) — bp: The health panel takes the retained look, effects kept.
3. task-452-03-hud-placement (vault/reference/tasks/task-452-health-above-weapons/task-452-03-hud-placement.md) — bp: The health panel sits just above the weapon slots.
4. task-452-04-doc (vault/reference/tasks/task-452-health-above-weapons/task-452-04-doc.md) — doc: Docs describe the new place and look.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-452-01-art]] — step 1, art.
- [[task-452-02-health-panel]] — step 2, bp.
- [[task-452-03-hud-placement]] — step 3, bp.
- [[task-452-04-doc]] — step 4, doc.
