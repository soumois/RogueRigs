---
id: task-455-xp-bar-ticks
description: "Global task: Thin light ticks split the XP bar into 5 equal parts."
parent: tasks-done-index
kind: task
status: done
---
# XP bar split in 5 by light ticks

- request: Owner 2026-11-05 (verbatim): "Il faudrait retravailler les différents UI in game , j'aimerais que la vie se trouve juste au dessus des armes. Owner answer 2026-11-05 (relayed by the main session, verbatim): "Q33 - Pas dedans, il faudrait que ce qui enveloppe le radar devra englober le nombre de minerai. ( voir ex en image ) Je répéte, c'est un exemple, pas à reprendre tel quel. Ok pour le reste ( les valeurs par défaut )" = the plan and the defaults of Q31 accepted.

Il faudrait refaire la barre d'évolution ( où on voit le swarm qui arrive et le C ( pour commande ) et  B pour bonus ), que ça soit mieux intégré

Il faudrait que le premier swarm soit fixe et que le second ne bouge pas avant que le premier ne soit finit. ( donc tant que le premier swarm n'est pas fini, les swarm ne réagissent pas au nombre de kill )

La barre d'xp, il faudrait une séparation légére par 5 et lorsqu'on gagne de l'xp, il faudrait un effet fantome de l'xp ( un peu comme quand on perds des pv, mais à l'inverse ).

Il faudrait aussi quelque chose pour entouré le radar qu'il fasse mieux intégré ( qu'un simple cercle ). Peut être que ce morceau d'interface inclut le nombre de minerai ( plutot que de flotter juste à coté ).

Le but va d'être faire une interface unifiée plutôt que chaque des petits morceaux d'interface séparée, j'attends des propositions visuelles de ta part en utilisant l'api chatgpt." Clarified the same day (relayed): "l'api chatgpt" is the Kie.ai key (KIE_API_KEY) with its GPT image model, i.e. Tools/art/kie_image.js. His screenshot of the current HUD: Art/Source/hud-unified/reference/owner-hud-current-2026-11-05.webp (radar top left with the ore count floating at its right, health bar under the radar, alert bar top centre with two red swarm marks and C, B, S letters, 4 weapon slots bottom centre, XP bar along the bottom edge with "Lv 1"). This task is the point "La barre d'xp, il faudrait une séparation légére par 5".
- goal: The XP bar shows light ticks at each fifth.
- acceptance: A0 Starts only once task-451-hud-mockups is done: the look comes from the mockup the owner retained there (recorded in its art-prompts node, files under Art/Source/hud-unified/). A1 per Q31 (default: 4 thin, light lines at 20, 40, 60, 80 % of the bar, whatever the level), above the fill and readable on empty and full parts without hiding it; A2 tick look per the retained mockup; A3 XP fill, "Lv N" and level-up behaviour unchanged; A4 Widgets follow umg-layout (root Canvas, anchors, no fixed pixel sizes) and start from the assets as they are: an uncommitted owner edit is committed alone first. the ticks follow the bar width at every resolution; A5 compiles and runs with no error; A6 No capture run by the capture agent unless the owner asks (Q26): he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it. The report also gives the token cost of this task, per subtask and in total, from cost-log.
- gdd_check: Read xp-level-up (HUD: cyan XP bar across the bottom, level on its left). No conflict; xp-level-up edited at confirmation. "par 5" has two readings, asked as Q31.
- confirmed_by_owner: yes (2026-11-05, "Q33 - Pas dedans, il faudrait que ce qui enveloppe le radar devra englober le nombre de minerai. ( voir ex en image ) Je répéte, c'est un exemple, pas à reprendre tel quel. Ok pour le reste ( les valeurs par défaut )")
- milestone: M2
- next_step: none
- started: 2026-11-06T00:57:36Z
- waiting_for: none
- result: Done. Commit 6ee90430: wbp-xp-status updated (tree XPBarArea > XPBar + XPTick20/40/60/80, ticks decision Q31, anchors table per umg-layout for each widget, verified line, umg-layout relation). Lint clean.

## Questions
Decided 2026-11-05 (owner "Q33 - Pas dedans, il faudrait que ce qui enveloppe le radar devra englober le nombre de minerai. ( voir ex en image ) Je répéte, c'est un exemple, pas à reprendre tel quel. Ok pour le reste ( les valeurs par défaut )"): every default below is accepted; values provisional, owner 2026-11-05.
- Q31 « Séparation par 5 ». Default: 4 traits fins et légers coupent la barre en 5 parts égales (20 % chacune), quel que soit le niveau. Autre : un trait tous les 5 points d'XP (leur nombre change avec le niveau).

## Subtasks
1. task-455-01-ticks (vault/reference/tasks/task-455-xp-bar-ticks/task-455-01-ticks.md) — bp: The XP bar shows the 5-part ticks.
2. task-455-02-doc (vault/reference/tasks/task-455-xp-bar-ticks/task-455-02-doc.md) — doc: Docs describe the ticks.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-455-01-ticks]] — step 1, bp.
- [[task-455-02-doc]] — step 2, doc.
