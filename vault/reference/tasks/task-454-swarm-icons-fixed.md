---
id: task-454-swarm-icons-fixed
description: "Global task: Until the first horde front is over, kills no longer move the swarms on the alert bar; afterwards they do as now."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# First swarms ignore kills until the first front is over

- request: Owner 2026-11-05 (verbatim): "Il faudrait retravailler les différents UI in game , j'aimerais que la vie se trouve juste au dessus des armes.

Il faudrait refaire la barre d'évolution ( où on voit le swarm qui arrive et le C ( pour commande ) et  B pour bonus ), que ça soit mieux intégré

Il faudrait que le premier swarm soit fixe et que le second ne bouge pas avant que le premier ne soit finit. ( donc tant que le premier swarm n'est pas fini, les swarm ne réagissent pas au nombre de kill )

La barre d'xp, il faudrait une séparation légére par 5 et lorsqu'on gagne de l'xp, il faudrait un effet fantome de l'xp ( un peu comme quand on perds des pv, mais à l'inverse ).

Il faudrait aussi quelque chose pour entouré le radar qu'il fasse mieux intégré ( qu'un simple cercle ). Peut être que ce morceau d'interface inclut le nombre de minerai ( plutot que de flotter juste à coté ).

Le but va d'être faire une interface unifiée plutôt que chaque des petits morceaux d'interface séparée, j'attends des propositions visuelles de ta part en utilisant l'api chatgpt." Clarified the same day (relayed): "l'api chatgpt" is the Kie.ai key (KIE_API_KEY) with its GPT image model, i.e. Tools/art/kie_image.js. His screenshot of the current HUD: Art/Source/hud-unified/reference/owner-hud-current-2026-11-05.webp (radar top left with the ore count floating at its right, health bar under the radar, alert bar top centre with two red swarm marks and C, B, S letters, 4 weapon slots bottom centre, XP bar along the bottom edge with "Lv 1"). This task is the point "Il faudrait que le premier swarm soit fixe et que le second ne bouge pas avant que le premier ne soit finit. ( donc tant que le premier swarm n'est pas fini, les swarm ne réagissent pas au nombre de kill )".
- goal: From level start until the first horde is over, the two swarm marks stay still on the alert bar whatever the kills, and the first horde comes when the fill reaches its mark.
- acceptance: A1 per the reading chosen in Q29 (default: until the first horde front is over, kills add nothing to the alert bar, so the fill follows time alone and both swarm marks keep their place); A2 "over" per Q30; A3 after that point kills move the bar and the marks as today; A4 threat still counts every kill; console at 40 %, console pause, extraction and the horde run times are unchanged; A5 a test covers: kills before the end of the first front leave the bar and both marks unchanged, kills after move them; existing tests pass, only expectations this change breaks are updated; figures given in the result; A6 No capture run by the capture agent unless the owner asks (Q26): he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it. The report also gives the token cost of this task, per subtask and in total, from cost-log.
- gdd_check: Read level-flow (M1: each kill adds 0.5 s), difficulty (alert grows with time and kills, threat never pauses), enemies-spawn-rules (two hordes triggered by run time, fronts about 1:35 and 2:55), sys-hud-pressure (horde mark = Alert + SecondsUntilFront / AlertFillSeconds: it moves when kills speed the fill). Conflict: the request changes how kills feed the alert bar (Q29); level-flow edited at confirmation. Two readings asked as Q29. Independent of task-451.
- confirmed_by_owner: no (plan and questions handed back 2026-11-05)
- milestone: M2
- next_step: task-454-01-rule
- started: open
- waiting_for: owner answers to Q24 to Q34
- result: open

## Questions
Asked 2026-11-05, each with a default; open until the owner answers.
- Q29 Swarms fixes, deux lectures. Default (A): les hordes gardent leur heure (1:35 et 2:55) ; tant que le premier front n'est pas fini, les kills n'ajoutent rien à la barre d'alerte (la menace, elle, compte toujours les kills), donc la barre avance au temps seul et les deux icônes de swarm restent immobiles ; ensuite les kills font avancer la barre et les icônes comme aujourd'hui. Autre (B) : la première horde est posée à une place fixe sur la barre et arrive quand le remplissage l'atteint (les kills la font donc venir plus tôt, sans bouger l'icône) ; la seconde reste immobile jusqu'à la fin de la première.
- Q30 « Fini ». Default: quand le front de la première horde a fini d'apparaître (son icône passe à « faite »). Autre : quand tous ses robots sont tués.

## Subtasks
1. task-454-01-rule (vault/reference/tasks/task-454-swarm-icons-fixed/task-454-01-rule.md) — cpp: Kills stop moving the alert bar and the swarm marks until the first front is over.
2. task-454-02-doc (vault/reference/tasks/task-454-swarm-icons-fixed/task-454-02-doc.md) — doc: Docs state the new rule.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-454-01-rule]] — step 1, cpp.
- [[task-454-02-doc]] — step 2, doc.
