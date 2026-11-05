---
id: task-451-hud-mockups
description: "Global task: Image proposals of one unified in-game HUD, made with Kie.ai GPT Image from the current screenshot; the owner retains one before any build."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Unified in-game HUD: visual proposals

- request: Owner 2026-11-05 (verbatim): "Il faudrait retravailler les différents UI in game , j'aimerais que la vie se trouve juste au dessus des armes. Owner answer 2026-11-05 (relayed by the main session, verbatim): "Q33 - Pas dedans, il faudrait que ce qui enveloppe le radar devra englober le nombre de minerai. ( voir ex en image ) Je répéte, c'est un exemple, pas à reprendre tel quel. Ok pour le reste ( les valeurs par défaut )" = the plan and the defaults of Q24, Q25, Q26 accepted.

Il faudrait refaire la barre d'évolution ( où on voit le swarm qui arrive et le C ( pour commande ) et  B pour bonus ), que ça soit mieux intégré

Il faudrait que le premier swarm soit fixe et que le second ne bouge pas avant que le premier ne soit finit. ( donc tant que le premier swarm n'est pas fini, les swarm ne réagissent pas au nombre de kill )

La barre d'xp, il faudrait une séparation légére par 5 et lorsqu'on gagne de l'xp, il faudrait un effet fantome de l'xp ( un peu comme quand on perds des pv, mais à l'inverse ).

Il faudrait aussi quelque chose pour entouré le radar qu'il fasse mieux intégré ( qu'un simple cercle ). Peut être que ce morceau d'interface inclut le nombre de minerai ( plutot que de flotter juste à coté ).

Le but va d'être faire une interface unifiée plutôt que chaque des petits morceaux d'interface séparée, j'attends des propositions visuelles de ta part en utilisant l'api chatgpt." Clarified the same day (relayed): "l'api chatgpt" is the Kie.ai key (KIE_API_KEY) with its GPT image model, i.e. Tools/art/kie_image.js. His screenshot of the current HUD: Art/Source/hud-unified/reference/owner-hud-current-2026-11-05.webp (radar top left with the ore count floating at its right, health bar under the radar, alert bar top centre with two red swarm marks and C, B, S letters, 4 weapon slots bottom centre, XP bar along the bottom edge with "Lv 1"). This task is the point "faire une interface unifiée ... j'attends des propositions visuelles de ta part en utilisant l'api chatgpt".
- goal: The owner has seen proposals of a unified HUD and retained one, which the build tasks 452 to 458 follow.
- acceptance: A1 proposals generated with Tools/art/kie_image.js (GPT Image 2 via Kie.ai, KIE_API_KEY), image-to-image from Art/Source/hud-unified/reference/owner-hud-current-2026-11-05.webp, count and spread per Q24, each shown to the owner as a file; A2 each proposal shows the whole HUD as one family of pieces, with every point of the request visible: health bar just above the weapon slots, alert bar with swarm marks and console/bonus/shuttle marks better integrated, radar wrapped by a frame that extends into an attached part holding the ore count (not inside the radar circle; owner Q33 answer, idea from Art/Source/hud-unified/reference/external-example-radar-panel-2026-11-05.png (owner example from another game, a reference only, not to copy as is: a circular minimap whose metal frame runs on to the right into an attached panel holding the title and resource counts, plus a small badge on its left)), XP bar with its 5-part ticks; A3 the look respects art-style-guide, color-code and the Kessler-Voss look already retained for the shop screen, and invents no game text beyond the current labels; A4 prompts kept verbatim under Art/Source/hud-unified/prompts/, the retained image with its run log committed, the others left untracked, per kie-image-workflow; A5 a new art-prompts node records the prompts, runs, credits, the owner's choice in his words and what each build task takes from it; A6 if the owner asks for a correction, at most one correction round per his request; no build work in this task.
- gdd_check: Read run-index, difficulty, level-flow, xp-level-up, enemies-spawn-rules, art-audio-index, art-prompts-shop-screen-mockup, kie-image-workflow (tool runs GPT Image 2, so "api chatgpt" maps to it per the owner's clarification), umg-layout, sys-hud-pressure, sys-hud-health-bar. No gdd node holds the HUD layout; no conflict with the request. Split per owner rule: one global per point (451 proposals, 452 health, 453 alert bar, 454 swarm rule, 455 XP ticks, 456 XP ghost, 457 radar and ore, 458 weapon slots if Q34 yes). Captures: the owner validates in game himself (recent practice, tasks 449-450), asked as Q26.
- confirmed_by_owner: yes (2026-11-05, "Q33 - Pas dedans, il faudrait que ce qui enveloppe le radar devra englober le nombre de minerai. ( voir ex en image ) Je répéte, c'est un exemple, pas à reprendre tel quel. Ok pour le reste ( les valeurs par défaut )")
- milestone: M2
- next_step: task-451-01-mockups
- started: 2026-11-05T23:16:22Z
- waiting_for: none
- result: open

## Questions
Decided 2026-11-05 (owner "Q33 - Pas dedans, il faudrait que ce qui enveloppe le radar devra englober le nombre de minerai. ( voir ex en image ) Je répéte, c'est un exemple, pas à reprendre tel quel. Ok pour le reste ( les valeurs par défaut )"): every default below is accepted; values provisional, owner 2026-11-05.
- Q24 Propositions. Default: 3 directions visuelles différentes, 2 variantes chacune (6 images, environ 36 crédits Kie.ai), toutes en image-to-image depuis ta capture, chacune montrant tout le HUD unifié (radar + minerai, barre d'alerte, vie au-dessus des armes, barre d'XP avec traits) dans la famille de look du shop (métal gunmetal/teal, rouille, Kessler-Voss). Tu en retiens une, ou tu demandes une correction, avant toute construction.
- Q25 Ordre. Default: la maquette (451) d'abord ; 452, 453, 455, 456, 457, 458 attendent ta maquette retenue ; 454 (règle des swarms) n'en dépend pas et peut partir tout de suite.
- Q26 Vérification. Default: pas de capture par l'agent capture ; tu valides en jeu toi-même, chaque rapport liste ce qu'il faut regarder. Autre choix : capture aux 4 résolutions (1920x1080, 1280x720, 1920x1200, 2560x1080) à la fin de chaque tâche visuelle.

## Subtasks
1. task-451-01-mockups (vault/reference/tasks/task-451-hud-mockups/task-451-01-mockups.md) — art: Generate the unified HUD proposals from the current screenshot, show them, record the owner's choice.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-451-01-mockups]] — step 1, art.
