---
id: task-458-weapon-slots-style
description: "Global task: The frames of the 4 weapon slots take the retained unified look; their contents stay."
parent: tasks-done-index
kind: task
status: done
---
# Weapon slots in the unified look

- request: Owner 2026-11-05 (verbatim): "Il faudrait retravailler les différents UI in game , j'aimerais que la vie se trouve juste au dessus des armes. Owner answer 2026-11-05 (relayed by the main session, verbatim): "Q33 - Pas dedans, il faudrait que ce qui enveloppe le radar devra englober le nombre de minerai. ( voir ex en image ) Je répéte, c'est un exemple, pas à reprendre tel quel. Ok pour le reste ( les valeurs par défaut )" = the plan and the defaults of Q34 accepted.

Il faudrait refaire la barre d'évolution ( où on voit le swarm qui arrive et le C ( pour commande ) et  B pour bonus ), que ça soit mieux intégré

Il faudrait que le premier swarm soit fixe et que le second ne bouge pas avant que le premier ne soit finit. ( donc tant que le premier swarm n'est pas fini, les swarm ne réagissent pas au nombre de kill )

La barre d'xp, il faudrait une séparation légére par 5 et lorsqu'on gagne de l'xp, il faudrait un effet fantome de l'xp ( un peu comme quand on perds des pv, mais à l'inverse ).

Il faudrait aussi quelque chose pour entouré le radar qu'il fasse mieux intégré ( qu'un simple cercle ). Peut être que ce morceau d'interface inclut le nombre de minerai ( plutot que de flotter juste à coté ).

Le but va d'être faire une interface unifiée plutôt que chaque des petits morceaux d'interface séparée, j'attends des propositions visuelles de ta part en utilisant l'api chatgpt." Clarified the same day (relayed): "l'api chatgpt" is the Kie.ai key (KIE_API_KEY) with its GPT image model, i.e. Tools/art/kie_image.js. His screenshot of the current HUD: Art/Source/hud-unified/reference/owner-hud-current-2026-11-05.webp (radar top left with the ore count floating at its right, health bar under the radar, alert bar top centre with two red swarm marks and C, B, S letters, 4 weapon slots bottom centre, XP bar along the bottom edge with "Lv 1"). This task is the point "retravailler les différents UI in game ... faire une interface unifiée plutôt que chaque des petits morceaux d'interface séparée" (weapon slots, Q34).
- goal: The weapon slots match the rest of the unified HUD.
- acceptance: A0 Starts only once task-451-hud-mockups is done: the look comes from the mockup the owner retained there (recorded in its art-prompts node, files under Art/Source/hud-unified/). A1 slot frames follow the retained mockup; A2 icons, reload/ammo bar, padlocks and the card-pick flight to a slot work as before; A3 Widgets follow umg-layout (root Canvas, anchors, no fixed pixel sizes) and start from the assets as they are: an uncommitted owner edit is committed alone first. A4 compiles and runs with no error; A5 No capture run by the capture agent unless the owner asks (Q26): he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it. The report also gives the token cost of this task, per subtask and in total, from cost-log.
- gdd_check: Not named point by point by the owner; derived from "retravailler les différents UI in game" and "interface unifiée", hence Q34. placeholder-weapon-icons stay as they are.
- confirmed_by_owner: yes (2026-11-05, "Q33 - Pas dedans, il faudrait que ce qui enveloppe le radar devra englober le nombre de minerai. ( voir ex en image ) Je répéte, c'est un exemple, pas à reprendre tel quel. Ok pour le reste ( les valeurs par défaut )")
- milestone: M2
- next_step: none
- started: 2026-11-06T01:13:02Z
- waiting_for: none
- executor_tokens: input 104, cache write 201148, cache read 3908732, output 23612
- executor_usage: runs 1, turns 51, context start 18k peak 125k, resumes 1, 151 min, units 0.76 M
- planning_tokens: input 6, cache write 14516, cache read 191373, output 4457
- planning_usage: a63802cd "Plan unified in-game HUD" 1/8 (tasks 451, 452, 453, 454, 455, 456, 457, 458), units 0.06 M
- result: Done. The 4 weapon slot frames take the unified riveted look by reusing owner-retained sprites, no generation: Frame0..3 = T_LevelEnd_Frame box brush, bolted base SlotsBase = T_Shop_ButtonReroll as the health plate (commit 5c275436). Icons, reload/ammo bars, padlocks, Frame0..3 variables (card flight) and graphs unchanged; compiled, saved, PIE without error. Root VerticalBox and 150 x 90 cells kept as found. Look unverified on screen (Q26): the owner checks it in game. Docs: wbp-weapons, UI index, art-prompts-hud-unified-mockup.

## Questions
Decided 2026-11-05 (owner "Q33 - Pas dedans, il faudrait que ce qui enveloppe le radar devra englober le nombre de minerai. ( voir ex en image ) Je répéte, c'est un exemple, pas à reprendre tel quel. Ok pour le reste ( les valeurs par défaut )"): every default below is accepted; values provisional, owner 2026-11-05.
- Q34 Emplacements d'armes. Default: oui, les cadres des 4 emplacements prennent le style de la maquette retenue (icônes, recharge et cadenas inchangés). Autre : non, pas de tâche 458.

## Subtasks
1. task-458-01-art (vault/reference/tasks/task-458-weapon-slots-style/task-458-01-art.md) — art: Cut the weapon slot frame sprite of the retained mockup.
2. task-458-02-slots (vault/reference/tasks/task-458-weapon-slots-style/task-458-02-slots.md) — bp: The weapon slots take the retained frame.
3. task-458-03-doc (vault/reference/tasks/task-458-weapon-slots-style/task-458-03-doc.md) — doc: Docs describe the new slot look.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-458-01-art]] — step 1, art.
- [[task-458-02-slots]] — step 2, bp.
- [[task-458-03-doc]] — step 3, doc.
