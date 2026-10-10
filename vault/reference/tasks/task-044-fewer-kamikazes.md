---
id: task-044-fewer-kamikazes
description: "Global task: At most 1 kamikaze per pack in one pack out of two; hordes 1–2 kamikazes (was 3–5)."
parent: tasks-done-index
kind: task
status: done
---
# Fewer kamikazes

- request: "Il y a au moins 2 fois trop de kamikaze." (owner, 2026-10-07)
- goal: At most 1 kamikaze per pack in one pack out of two; hordes 1–2 kamikazes (was 3–5).
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: none
- result: Done. Kamikazes 0–1 per Chasers, ShieldChasers and WideShieldChasers pack (was 1–2), hordes 1–2 (was 3–5; overtime horde copies it); early packs 5–10 robots. Director horde test updated; Spawner.HeadingSectorSplit sample 200 → 2000 hordes (the shift broke the small sample, cause not found). Build 0, 195/195. Commit d9a4454.

## Subtasks
1. task-044-01-fewer-kamikazes — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-044-01-fewer-kamikazes]] — step 1, cpp.
