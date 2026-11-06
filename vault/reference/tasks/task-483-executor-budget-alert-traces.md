---
id: task-483-executor-budget-alert-traces
description: "Global task: Measures executors against their level: non-blocking budget alert at closing, and executor traces kept in agent-usage.js. Open to run or review the measurement."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Budget alert at closing and executor traces

- request: Owner, verbatim: "Ok, comme est-ce que tu proposes de mettre en place ce dernier point ?" (the point: proportion verification to the size of the change, and group small steps), then, on the proposal of the main session: "Oui, planifie". Accepted proposal, task B: alert at closing when a task exceeds the budget of its level: task-cost.js already gets the usage data for each closed subtask (called from record-cost.js); the alert must not block closing. Keep executor traces in agent-usage.js, as already done for planners and bp/cpp, so a future run can show where its turns went. Point to check at execution: usage-snapshot.json is already over 4.9 MB, and usage-agents.json (read by the crew-board mod, 4 MiB read cap) must stay without traces. The executor transcripts of tasks before 476 are gone from this machine and agentTrace is kept only for bp and cpp (agent-usage.js about line 283), so where 474's turns went cannot be established. Why, cost analysis of tasks 460-480 without cancelled 466 (agent-usage.js): 20 tasks = 37.52 M units, executors 92 %, cache reads 69 % of executor cost. task-474 (miner 10 % smaller) cost 1.49 M, 91 turns, peak context 171k, for one Blueprint value and one C++ line; 475 and 476 are similar (1.43 M each). Nothing brakes an executor today. Known limits: the expected gain is modest (about 2.4 M of 37.5 M, 6 %, on 474, 475, 476 at about 0.8 M each, an estimate); the risk is a defect reaching owner validation; budget figures are starting values. A later comparison on about ten tasks, reference 474, 475, 476 (1.33 to 1.41 M executor), is NOT a task unless the owner asks. Owner answer 2026-11-06 (relayed by the main session, verbatim): "Ok" = the plan and the defaults of Q6, Q7 accepted.
- goal: An executor over the budget of its level is reported at closing without blocking it, and a closed executor run can show where its turns went, including after its transcript is gone.
- acceptance: A1 Closing a subtask of a global whose executor run has passed the turns or the units of the budget of its level (task-482) prints an ALERT line giving level, budget and measured figures, on a task with several subtasks as on a task with one. A2 Closing still succeeds, with the same exit status and the same node changes as without the alert. A3 A global without a level prints no alert and closes as before. A4 The alert takes its figures from the place task-482 wrote them: changing a budget takes one edit. A5 agent-executor cost_alerts says the Executor writes the cause in result and lists the alert in its final report. A6 A new executor run has a trace in usage-snapshot.json and the detail option of agent-usage.js shows it, also when the transcript no longer exists. A7 usage-agents.json still holds no trace and stays under 4 MiB. A8 The result gives the size of one real executor trace and of usage-snapshot.json. If a new trace would take usage-snapshot.json over 20 MB, the oldest executor traces are removed and their numbers stay. A9 Lint clean, committed by path, no game code, Blueprint or asset changed, no capture.
- gdd_check: No gdd node touched: this is crew process, outside the game milestones, so no gdd conflict and no milestone scope issue. Crew nodes read: agent-planner, agent-executor, orchestration-protocol, orchestrator-lessons, task-spec. Two targeted code reads, reason: an alert per closed subtask looks unusable with zero subtask rows (agent-executor cost). Fact: task-cost.js already computes turns and units of the whole executor run and writes them on the global at each closed subtask, so the alert compares that running total to the budget. Limit: turns after the last closure are not counted (Q7). agent-usage.js already strips traces from usage-agents.json; line 283 is the bp and cpp restriction. No open value besides the size limit (Q6). Depends on task-482 (level field and figures): run it after 482 is done. Do not run alongside task-481 or another executor: closing subtasks rewrites usage-snapshot.json.
- confirmed_by_owner: yes (2026-11-06, "Ok")
- milestone: M2
- next_step: task-483-01-budget-alert
- started: 2026-11-06T22:48:40Z
- waiting_for: none
- result: open

## Questions
Decided 2026-11-06 (owner "Ok"): every default below is accepted; values provisional, owner 2026-11-06.
- Q6 Les traces d'executeurs font grossir usage-snapshot.json, committe et deja a 4,9 Mo. Quelle limite ? Default: 20 Mo ; au-dela on retire les traces d'executeurs les plus anciennes, leurs chiffres restent.
- Q7 L'alerte se declenche a la fermeture de chaque sous-tache, si les tours ou les unites depassent le budget, sur le total de l'execution a cet instant ; les derniers tours apres la derniere fermeture ne comptent pas. Ca te va ? L'Executor en donne la cause dans result et la cite dans son rapport final. Default: oui ; une tache sans niveau ne declenche aucune alerte.

## Subtasks
1. task-483-01-budget-alert (vault/reference/tasks/task-483-executor-budget-alert-traces/task-483-01-budget-alert.md) — doc: Print a non-blocking alert at closing when the executor run exceeds the budget of its level.
2. task-483-02-executor-traces (vault/reference/tasks/task-483-executor-budget-alert-traces/task-483-02-executor-traces.md) — doc: Keep executor traces in agent-usage.js under the size limit, with usage-agents.json still free of them.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-483-01-budget-alert]] — step 1, doc.
- [[task-483-02-executor-traces]] — step 2, doc.
