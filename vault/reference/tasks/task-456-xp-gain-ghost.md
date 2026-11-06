---
id: task-456-xp-gain-ghost
description: "Global task: on an XP gain a pale ghost fill shows the new XP at once; the cyan fill then catches up."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# XP gain ghost on the XP bar

- request: Owner 2026-11-05 (verbatim): "Il faudrait retravailler les différents UI in game , j'aimerais que la vie se trouve juste au dessus des armes. Owner answer 2026-11-05 (relayed by the main session, verbatim): "Q33 - Pas dedans, il faudrait que ce qui enveloppe le radar devra englober le nombre de minerai. ( voir ex en image ) Je répéte, c'est un exemple, pas à reprendre tel quel. Ok pour le reste ( les valeurs par défaut )" = the plan and the defaults of Q32 accepted.

Il faudrait refaire la barre d'évolution ( où on voit le swarm qui arrive et le C ( pour commande ) et  B pour bonus ), que ça soit mieux intégré

Il faudrait que le premier swarm soit fixe et que le second ne bouge pas avant que le premier ne soit finit. ( donc tant que le premier swarm n'est pas fini, les swarm ne réagissent pas au nombre de kill )

La barre d'xp, il faudrait une séparation légére par 5 et lorsqu'on gagne de l'xp, il faudrait un effet fantome de l'xp ( un peu comme quand on perds des pv, mais à l'inverse ).

Il faudrait aussi quelque chose pour entouré le radar qu'il fasse mieux intégré ( qu'un simple cercle ). Peut être que ce morceau d'interface inclut le nombre de minerai ( plutot que de flotter juste à coté ).

Le but va d'être faire une interface unifiée plutôt que chaque des petits morceaux d'interface séparée, j'attends des propositions visuelles de ta part en utilisant l'api chatgpt." Clarified the same day (relayed): "l'api chatgpt" is the Kie.ai key (KIE_API_KEY) with its GPT image model, i.e. Tools/art/kie_image.js. His screenshot of the current HUD: Art/Source/hud-unified/reference/owner-hud-current-2026-11-05.webp (radar top left with the ore count floating at its right, health bar under the radar, alert bar top centre with two red swarm marks and C, B, S letters, 4 weapon slots bottom centre, XP bar along the bottom edge with "Lv 1"). This task is the point "lorsqu'on gagne de l'xp, il faudrait un effet fantome de l'xp ( un peu comme quand on perds des pv, mais à l'inverse )".
- goal: Gaining XP is seen as a ghost that the fill then catches up.
- acceptance: A0 Starts only once task-451-hud-mockups is done: the look comes from the mockup the owner retained there (recorded in its art-prompts node, files under Art/Source/hud-unified/). A1 per Q32: on a gain the ghost fill shows the new value at once, the cyan fill catches up after about 0.4 s; a new gain restarts the wait; A2 on a level-up the ghost reaches the end, the bar empties, then the remainder shows the same way; several levels at once do not leave the bar stuck; A3 no per-frame work when nothing moves (event armed, as the health bar); A4 ticks of task-455 and "Lv N" unchanged; colour and look per the retained mockup; A5 a test covers ghost and fill values after a gain and after a level-up; existing tests pass, only expectations this change breaks are updated; A6 No capture run by the capture agent unless the owner asks (Q26): he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it. The report also gives the token cost of this task, per subtask and in total, from cost-log.
- gdd_check: Read xp-level-up, sys-hud-health-bar (chunk: delay 0.4 s, rate 7 per s, event armed): the ghost mirrors it. No conflict; xp-level-up edited at confirmation.
- confirmed_by_owner: yes (2026-11-05, "Q33 - Pas dedans, il faudrait que ce qui enveloppe le radar devra englober le nombre de minerai. ( voir ex en image ) Je répéte, c'est un exemple, pas à reprendre tel quel. Ok pour le reste ( les valeurs par défaut )")
- milestone: M2
- next_step: task-456-02-ghost-bar
- started: 2026-11-06T01:01:19Z
- waiting_for: none
- result: open

## Questions
Decided 2026-11-05 (owner "Q33 - Pas dedans, il faudrait que ce qui enveloppe le radar devra englober le nombre de minerai. ( voir ex en image ) Je répéte, c'est un exemple, pas à reprendre tel quel. Ok pour le reste ( les valeurs par défaut )"): every default below is accepted; values provisional, owner 2026-11-05.
- Q32 Fantôme d'XP. Default: à chaque gain, un remplissage pâle (cyan très clair) montre tout de suite la nouvelle XP, puis la barre cyan le rattrape après environ 0,4 s, au même rythme que le morceau clair de la vie ; au level up, le fantôme va au bout, la barre se vide puis le reste s'affiche de la même façon.

## Subtasks
1. task-456-01-ghost-logic (vault/reference/tasks/task-456-xp-gain-ghost/task-456-01-ghost-logic.md) — cpp: The XP bar drives a ghost fill and a delayed catch-up from XP events.
2. task-456-02-ghost-bar (vault/reference/tasks/task-456-xp-gain-ghost/task-456-02-ghost-bar.md) — bp: The XP widget gets the ghost bar behind the fill, in the retained look.
3. task-456-03-doc (vault/reference/tasks/task-456-xp-gain-ghost/task-456-03-doc.md) — doc: Docs describe the ghost.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-456-01-ghost-logic]] — step 1, cpp.
- [[task-456-02-ghost-bar]] — step 2, bp.
- [[task-456-03-doc]] — step 3, doc.
