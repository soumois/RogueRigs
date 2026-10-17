---
id: task-228-xp-big-pop
description: "Give the big XP cube the same floaty pop (projection) as the XP nut: BP_PickupXPBig pop values copied from BP_PickupXP."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Big XP: floaty pop like the nut

- request: "Et tu peux mettre les effets de projection des collectables sur le gros XP (comme on l'avait désactivé pour la puce, je ne sais pas si tu l'avais remis lorsqu'on est passé au cube)." (owner 2026-10-17). It was not: task-195-02 set the floaty pop on BP_PickupXP only and BP_PickupXPBig kept 0.35 s.
- goal: BP_PickupXPBig pops like BP_PickupXP.
- acceptance: pop values read back equal to BP_PickupXP; doc updated.
- gdd_check: [[bp-pickup-xp]] (floaty_pop: PopDuration 1.6 s, PopArcHeight 250, PopMinDistance 80, PopMaxDistance 300, PopFallTimeRatio 2, bAttractDuringPop true, PopAttractDelay 1.2 s, PopBounceFraction 0.25), [[sys-pickups-lifecycle]]. The flight spin and physics landing come with [[task-229-pickup-physics]] for every pickup.
- confirmed_by_owner: yes
- milestone: M1
- next_step: task-228-01-bp-pickup-xp-big-pop
- result: open

## Subtasks
- [[task-228-01-bp-pickup-xp-big-pop]] — bp: copy the pop values to BP_PickupXPBig.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[bp-pickup-xp]] — the Blueprints.
