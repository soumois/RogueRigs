---
id: task-293-heavy-slam-bigger-slower
description: "Global task: Heavy slam radius 289 to 361 uu and telegraph 0.6 to 1.0 s, fist raise stretched to the longer telegraph."
parent: tasks-to-do-index
kind: task
status: todo
---
# Heavy slam 25 % larger, 1 s wind-up

- request: "Agrandit son attaque de 25% mais passe sa préparation à 1 sec" (owner, 2026-10-23)
- goal: EnemyStats Heavy AttackRange 361 (ground telegraph follows GetSlamRadius), WindUp 1.0; trigger ratio 0.7 kept (253 uu); the fist raise of clip 1 lasts until the strike instead of 0.45 s; strike and recover clips unchanged; slam contact still on the slam frame (SlamContactLead checked) (provisional, owner 2026-10-23).
- acceptance: CSV values read back; telegraph circle 361 uu in game; raise reaches the top just before the strike; fists touch the ground on the slam frame; broken heavy tests updated and passing; -game capture of a telegraph and slam; committed.
- gdd_check: enemy-heavy read (2026-10-23): slam radius 361, telegraph 1.0 s, raise over 0.85 s, trigger 253 uu written (provisional, owner 2026-10-23). Note: the telegraph clip is 15 rows played at 30 fps (0.47 s max), so a rebake alone cannot stretch it; route chosen: C++ drives the clip-1 frame from the telegraph progress (no shader change); the raise then steps at about 17 poses per second.
- confirmed_by_owner: yes (2026-10-23, "Le reste est bon"; planned in a planning pass, each task run later by its own orchestrator)
- milestone: M2
- next_step: task-293-01-cpp-heavy-slam-values
- result: open

## Subtasks
1. task-293-01-cpp-heavy-slam-values — cpp: Heavy slam radius and wind-up
2. task-293-02-capture-heavy-slam — cpp: Capture the heavy telegraph and slam

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-293-01-cpp-heavy-slam-values]] — step 01, cpp.
- [[task-293-02-capture-heavy-slam]] — step 02, cpp.
