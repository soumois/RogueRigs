---
id: task-185-alert-label-remove
description: "Global task: remove the ALERT text next to the alert bar, and the one test check that reads it."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Alert bar: remove the ALERT text

- request: Prépare une tâche aussi pour enlever le texte "Alert" à côté de la barre d'alerte (owner, 2026-10-14)
- goal: The alert bar shows without the ALERT / ALERT (paused) text; nothing else of the alert bar changes.
- acceptance: AlertLabel gone from WBP_AlertObjectives and its graphs; the integration test no longer reads it; build and run integration tests pass except known failures; docs updated; committed.
- gdd_check: readability-rules (alert bar line) and wbp-alert-objectives read (2026-10-14): the GDD does not ask for the label, no conflict; the label also showed the pause ("ALERT (paused)"): default, no replacement indicator, to confirm with the owner.
- confirmed_by_owner: yes (2026-10-15); no pause indicator
- milestone: M2
- next_step: task-185-01-alert-label-widget
- result: open

## Subtasks
1. task-185-01-alert-label-widget — bp
2. task-185-02-alert-label-test — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-185-01-alert-label-widget]] — step 1, bp.
- [[task-185-02-alert-label-test]] — step 2, cpp.
