---
id: task-010-damage-numbers-video
description: "Global task: quick damage numbers above every enemy, reusing the dummy's floating text, for the owner's assignment video."
parent: tasks-index
kind: task
status: done
---
# Damage numbers on enemies (quick version)

- request: "Les dégâts n'apparaissent pas sur les chasers. Il faudrait que ce soit dans le code commun pour que, lorsqu'ils prennent des dégâts, cela s'affiche au-dessus d'eux." Then: "Tu saurais mettre l'affichage des dégâts, juste le temps de faire une vidéo pour un assignment ?" Owner asked for the quick version now (2026-10-02).
- goal: every `ARREnemy` shows the existing floating damage text above its head on each hit (white, bigger white-gold on crits).
- acceptance: (1) build exits 0; all `RogueRigs.*` headless tests pass, plus one test that a hit chaser spawns a floating text above its capsule; (2) [[sys-enemies]] updated; (3) owner sees numbers on chasers in PIE.
- gdd_check: [[damage-numbers]] is M2 (Niagara at horde scale), M1 debug text was dummy-only → owner extends the debug text to all enemies (2026-10-02). Risk: one actor per hit does not scale to 200 robots; the shared feedback component with a text pool stays a later task. Merge window open: not merged.
- confirmed_by_owner: yes (2026-10-02)
- milestone: M1
- next_step: none
- result: acceptance 1 and 2 passed (36/36 headless); owner used it (2026-10-02). Shared feedback component with a text pool remains a later task.

## Subtasks
1. task-010-01-enemy-damage-text — cpp: floating damage text on `ARREnemy`.

## Relations
- [[tasks-index]] — parent index.
- [[task-010-01-enemy-damage-text]] — step 1, cpp.
