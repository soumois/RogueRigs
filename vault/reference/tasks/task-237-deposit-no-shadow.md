---
id: task-237-deposit-no-shadow
description: "Global task: Ore and heal deposit formations cast no shadow."
parent: tasks-done-index
kind: task
status: done
---
# Deposit formations without shadows

- request: "Retire les ombres aussi sur les cristaux" ; pas de capture, l'owner vérifie (owner, 2026-10-19)
- goal: Ore and heal deposit formations cast no shadow.
- acceptance: build exits 0 editor closed; RogueRigs.* tests pass except the 2 known pre-existing failures; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-19).
- confirmed_by_owner: yes (2026-10-19)
- milestone: M2
- next_step: none
- result: Done. Commits 1513608e, 60a571ca, 5d8e7471, 297de7ee. bDepositCrystalsCastShadow (default false) turns off cast, dynamic, static and contact shadows on deposit formation ISMs and the cluster fallback. RockDamageData made aware of heal deposit formations (broken by 234-04) and checks CastShadow false. Build 0; Dig 13/13; full run 262/264 (2 known). Look checked by the owner.

## Subtasks
1. task-237-01-deposit-no-shadow — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-237-01-deposit-no-shadow]] — step 1, cpp.
