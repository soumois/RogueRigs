---
id: task-195-floaty-pop-midair-magnet
description: "Global task: long floaty pop arc and mid-air magnet after a delay for XP nut, ore and heal pickups, not the big chip."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Floaty pop and mid-air magnet

- request: @video 2026-10-06 10-28-18.mkv, 00:13 à 00:25 regarde comme l'xp est envoyé dans les air, je pense qu'on pourrait faire pareil avec les écroux (...) à mon avis, les xp ont une gravité un peu spécial. Et surtout (comme ça prend du temps à retomber au sol), l'attraction peut s'activer même quand c'est encore en l'air (mais pas tout de suite, peut être aprés 1 ou 1.5sec); then: tous sauf la puce; acceptable pour l'instant le fait que ça retombe sur la roche; defaults accepted (owner, 2026-10-15)
- goal: Pickup pop: long floaty arc (slower fall than rise) and magnet allowed mid-air after a delay from spawn, for XP nut, ore and heal pickups, not the chip.
- acceptance: XP nut, ore and heal pickups fly about 1.6 s on a 250 uu arc with a slower fall, up to 300 uu away, and can be attracted mid-air 1.2 s after spawn; the big chip is unchanged; build clean; existing tests pass; docs updated; lint clean; committed.
- gdd_check: xp-level-up and ore-blocks read 2026-10-15. Conflicts decided by the owner: short arcs and the 1 s rest before the magnet replaced by a long floaty arc and a mid-air magnet after 1.2 s from spawn (xp-level-up updated); ore and heal chunks may now land on unbroken rock, accepted for now (ore-blocks updated). The big XP chip keeps today's pop. Values provisional (owner defaults accepted).
- confirmed_by_owner: yes (2026-10-15: "ok pour les deux tâches avec ces valeurs", scope "tous sauf la puce")
- milestone: M1
- next_step: task-195-02-floaty-pop-values
- result: open

## Subtasks
1. task-195-01-floaty-pop-midair-magnet — cpp
2. task-195-02-floaty-pop-values — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-195-01-floaty-pop-midair-magnet]] — step 1, cpp.
- [[task-195-02-floaty-pop-values]] — step 2, bp.
- [[xp-level-up]] — the rule changed by this task.
