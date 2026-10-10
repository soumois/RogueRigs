---
id: task-083-voice-tweak
description: "Global task: upgrade lines regenerated with an item-specific retrieval and compared with v1, a real tweak for the README."
parent: tasks-done-index
kind: task
status: done
---
# A4 voice tweak (upgrade lines v2)

- request: "tu peux lancer le reste des tâches pour le devoir 4" (owner playtest 2026-10-10: shuttle line "Cease all extraction" misread, card flavor line not visible) (owner, 2026-10-10)
- goal: A4 voice tweak: upgrade flavor lines regenerated with an item-specific retrieval, compared with the first version, so the README shows a real prompt or retrieval tweak.
- acceptance: validate_text.js exits 0 on v2; v1 and v2 side by side in the logs; critic verdict on 13 items; the tweak recorded; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-10).
- confirmed_by_owner: yes (2026-10-10)
- milestone: M2
- next_step: none
- result: Done. v2 upgrade lines (per-stat retrieval): critic round 0: 10 pass 3 fail (T3 x2, T2), corrected; round 1: 13 pass; adopted in Content/, v1 kept in Tools/content/logs/v1. Run 20261010-112603-upgrade_texts.

## Subtasks
1. task-083-01-voice-tweak — text
2. task-083-02-voice-tweak — style-guide

## Relations
- [[tasks-done-index]] — parent index.
- [[task-083-01-voice-tweak]] — step 1, text.
- [[task-083-02-voice-tweak]] — step 2, style-guide.
