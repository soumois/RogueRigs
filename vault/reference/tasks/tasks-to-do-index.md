---
id: tasks-to-do-index
description: "Open global tasks (in progress, wait-answers, todo, review, blocked), one line each with status. Read on every Orchestrator start."
parent: tasks-index
kind: note
---
# Tasks to do

A global task moves to [[tasks-done-index]] when it is done or cancelled (`vault/scripts/task-status.js` does it). A global waiting for the owner stays here as `Status wait-answers.`; its node's `waiting_for` says what it waits for.

## Relations
- [[tasks-index]] — parent index.
- [[task-500-request-families]] — Crew workflow: the relay classifies a request (réglage, comportement, visuel, gros morceau); réglage skips the planner; gros morceau is split into several globals. Status wait-answers.
- [[task-501-visual-preview-gate]] — Crew workflow: a visual or taste request plans a mockup or one slice with one capture first; the executor waits for the owner go before the full build. Status wait-answers.
- [[task-502-executor-handover-budget]] — Crew workflow: at a subtask boundary over the context threshold the executor hands over to a fresh one; reaching the level budget triggers a stop and a question to the owner. Status wait-answers.
- [[task-503-executor-reading-list]] — Crew workflow: the executor reads runbook and quirks only when a script fails; the specialist agent definitions in .claude/agents are handled as the owner decides; no vault node is merged. Status wait-answers.
