---
id: task-457-radar-frame-ore
description: "Global task: The radar gets a frame in the unified look, and the ore count moves into that frame instead of floating beside it."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Radar frame holding the ore count

- request: Owner 2026-11-05 (verbatim): "Il faudrait retravailler les différents UI in game , j'aimerais que la vie se trouve juste au dessus des armes.

Il faudrait refaire la barre d'évolution ( où on voit le swarm qui arrive et le C ( pour commande ) et  B pour bonus ), que ça soit mieux intégré

Il faudrait que le premier swarm soit fixe et que le second ne bouge pas avant que le premier ne soit finit. ( donc tant que le premier swarm n'est pas fini, les swarm ne réagissent pas au nombre de kill )

La barre d'xp, il faudrait une séparation légére par 5 et lorsqu'on gagne de l'xp, il faudrait un effet fantome de l'xp ( un peu comme quand on perds des pv, mais à l'inverse ).

Il faudrait aussi quelque chose pour entouré le radar qu'il fasse mieux intégré ( qu'un simple cercle ). Peut être que ce morceau d'interface inclut le nombre de minerai ( plutot que de flotter juste à coté ).

Le but va d'être faire une interface unifiée plutôt que chaque des petits morceaux d'interface séparée, j'attends des propositions visuelles de ta part en utilisant l'api chatgpt." Clarified the same day (relayed): "l'api chatgpt" is the Kie.ai key (KIE_API_KEY) with its GPT image model, i.e. Tools/art/kie_image.js. His screenshot of the current HUD: Art/Source/hud-unified/reference/owner-hud-current-2026-11-05.webp (radar top left with the ore count floating at its right, health bar under the radar, alert bar top centre with two red swarm marks and C, B, S letters, 4 weapon slots bottom centre, XP bar along the bottom edge with "Lv 1"). This task is the point "quelque chose pour entouré le radar qu'il fasse mieux intégré ( qu'un simple cercle ). Peut être que ce morceau d'interface inclut le nombre de minerai ( plutot que de flotter juste à coté )".
- goal: Radar and ore count read as one framed HUD piece.
- acceptance: A0 Starts only once task-451-hud-mockups is done: the look comes from the mockup the owner retained there (recorded in its art-prompts node, files under Art/Source/hud-unified/). A1 per Q33: a frame surrounds the radar, look per the retained mockup; A2 the ore icon and count sit inside that frame; nothing floats beside the radar any more; A3 radar content, ore count updates and anything that pointed at the old counter work as before; A4 Widgets follow umg-layout (root Canvas, anchors, no fixed pixel sizes) and start from the assets as they are: an uncommitted owner edit is committed alone first. A5 compiles and runs with no error, existing tests pass, only expectations this change breaks are updated; A6 No capture run by the capture agent unless the owner asks (Q26): he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it. The report also gives the token cost of this task, per subtask and in total, from cost-log.
- gdd_check: Read umg-layout. No gdd node places the radar or the ore count; nothing conflicts.
- confirmed_by_owner: no (plan and questions handed back 2026-11-05)
- milestone: M2
- next_step: task-457-01-art
- started: open
- waiting_for: owner answers to Q24 to Q34
- result: open

## Questions
Asked 2026-11-05, each with a default; open until the owner answers.
- Q33 Radar. Default: un cadre (look de la maquette retenue) autour du radar, avec le compteur de minerai (icône + nombre) intégré dans ce cadre ; le compteur flottant disparaît ; tout ce qui visait l'ancien compteur vise le nouveau.

## Subtasks
1. task-457-01-art (vault/reference/tasks/task-457-radar-frame-ore/task-457-01-art.md) — art: Cut the radar frame sprite of the retained mockup.
2. task-457-02-radar-frame (vault/reference/tasks/task-457-radar-frame-ore/task-457-02-radar-frame.md) — bp: The radar widget gets its frame with a place for the ore count.
3. task-457-03-ore-in-frame (vault/reference/tasks/task-457-radar-frame-ore/task-457-03-ore-in-frame.md) — bp: The ore count shows inside the radar frame; the floating counter is gone.
4. task-457-04-doc (vault/reference/tasks/task-457-radar-frame-ore/task-457-04-doc.md) — doc: Docs describe the framed radar and the ore count place.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-457-01-art]] — step 1, art.
- [[task-457-02-radar-frame]] — step 2, bp.
- [[task-457-03-ore-in-frame]] — step 3, bp.
- [[task-457-04-doc]] — step 4, doc.
