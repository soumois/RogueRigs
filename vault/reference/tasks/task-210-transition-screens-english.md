---
id: task-210-transition-screens-english
description: "Global task: Intermission and loading screens in English (T7)."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Intermission and loading screens in English (T7)

- request: Owner "ok" (2026-10-17) to the T7 conflict found by the orchestrator: WBP_Intermission and WBP_Loading were built with French texts while every on-screen text must be English (owner, 2026-10-17)
- goal: the intermission and loading screens show English texts written by the text agent, to follow rule T7.
- acceptance: both widgets show English texts that passed the style-guide review (or kept deviations recorded); docs updated; captures sent to the owner; lint clean; committed.
- gdd_check: [[dialogue-tone]] T7 (every on-screen text in English), [[intermission]]. Conflict with task-206-04 and 206-05 (French), owner 2026-10-17: translate to English.
- confirmed_by_owner: yes (2026-10-17)
- milestone: M2
- next_step: task-210-03-intermission-bp
- result: open

## Subtasks
1. task-210-01-transition-copy — text
2. task-210-02-transition-review — style-guide
3. task-210-03-intermission-bp — bp
4. task-210-04-loading-bp — bp
5. task-210-05-capture — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-206-level-to-level]] — the transition this fixes.
- [[task-210-01-transition-copy]] — step 1, text.
- [[task-210-02-transition-review]] — step 2, style-guide.
- [[task-210-03-intermission-bp]] — step 3, bp.
- [[task-210-04-loading-bp]] — step 4, bp.
- [[task-210-05-capture]] — step 5, cpp.
