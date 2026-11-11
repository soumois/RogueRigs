---
id: task-502-executor-handover-budget
description: "Global task: executor hands over to a fresh one over a context threshold; reaching the level budget stops and asks the owner."
parent: tasks-done-index
kind: task
status: done
---
# Executor handover over a context threshold and a budget that triggers

- request: Owner 2026-11-11, verbatim: "ok, prépare la tâche pour mettre ça en place." Earlier in the same conversation, on removing the specialist agent definitions and merging executor reading nodes: "Oui, c'est juste que ça rentrait en conflit avec les régles du vault, ça n'est pas un soucis ?" On ruflo: "Non, ruflo est apparu car j'ai changé de pc et j'avais un plugin activé, mais je ne veux pas ça." "ça" = the main session proposal he accepted: keep the base (planner, one fresh Opus executor per global, style-guide critic, vault as memory) but route each request by family at the entry, picked by the relay from the owner sentence or named by the owner. (1) Réglage (task-490, 491, 474): no planner, one executor on the owner words, a script writes the task node for the trace (a plan costs 0.36 M on average, a light task 0.15 to 0.3 M). (2) Comportement ou système (task-496, 497, 493): planner then executor, plus a handover to a fresh executor at a subtask boundary over 120k context, and a budget that triggers something instead of being indicative. (3) Visuel ou goût (task-499, 461/467/477, 462/466): preview first (mockup or one slice, one capture, owner go), then the full build (task-499: 6.57 M refused at the capture and reverted). (4) Gros morceau (task-484 at 7 M, task-424): the planner splits into several globals, one fresh executor each. Not wanted: specialists per trade, a verifier agent; the only delegation kept is style-guide. Prepare only, launch nothing. Owner answer 2026-11-11 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q6, Q7, Q8 accepted.
- goal: A long global no longer runs in one growing context, and reaching the budget of its level makes the owner decide instead of being only noted.
- acceptance: A1 A script prints the current executor context in k tokens, and task-next.js prints at each subtask closing "context: <n>k" and, over the threshold of Q6 with subtasks left, a HANDOVER line. A2 On HANDOVER the executor writes a "## Handover" section in the global node (max 300 words: done, state, next, pitfalls), commits and stops; task.md tells the relay to launch a fresh executor that starts from that section. A3 Budget: reaching the budget of the level at a closing triggers what Q7 decides; agent-executor budget, verification-levels and execution-protocol say so, and no node still says the budget never blocks. A4 Checked by running the scripts on an existing executor transcript or usage trace (the context figure and both lines appear when forced). A5 Lint clean, committed.
- gdd_check: No gdd node touched: the request changes the dev crew (agents branch). Read: agent-planner, orchestration-protocol, orchestrator-lessons, task-spec, agent-executor, execution-protocol, verification-levels. No code read. Facts from the main session, not re-checked: rapport.md section 7 (2026-11-07) measured the 120k handover at -21 % on task-495 (one run) on branch exp/crew-tools, which no longer exists: rebuild, do not merge. Since then task-496 (standard, budget 90 turns 1.5 M) ran 128 turns, peak 339k, 3.66 M; task-499 cost 6.57 M and was cancelled. Rule nodes orchestration-protocol, execution-protocol and verification-levels (kind: rule) are edited under this owner intent once confirmed. _authoring constraint raised by the owner: no node merge; one concept per node.
- verification_level: standard
- confirmed_by_owner: yes (2026-11-11, "ok")
- milestone: M2
- next_step: none
- started: 2026-11-11T21:17:38Z
- waiting_for: none
- executor_tokens: input 70, cache write 97194, cache read 2584163, output 25034
- executor_usage: runs 1, turns 35, context start 15k peak 109k, resumes 0, 4 min, units 0.51 M
- planning_tokens: input 5, cache write 12163, cache read 83526, output 4150
- planning_usage: add7b373 "Plan crew workflow routing change" 1/4 (tasks 500, 501, 502, 503), units 0.04 M
- result: Done. context-now.js prints the executor context (last turn of the newest executor transcript of the global, else the usage-snapshot trace); task-next.js prints context: <n>k and, over alerts.handoverContextTokens 120000 (cost-rates.json) with subtasks left, a HANDOVER line. BUDGET line from task-cost.js over the level budget times 1 + budget_grants (task-status.js inserts the field); execution-protocol steps 5.9 handover and 5.10 budget stop, agent-executor handover/budget/cost_alerts, verification-levels, runbook, cost-log, agents-index, task.md relay relaunch, executor.md step 8. A4 checked: task-502 live 86k, task-496 transcript 339k with HANDOVER, task-441 usage trace 318k with HANDOVER, RR_HANDOVER_TOKENS=20000 forces HANDOVER on the live run, BUDGET on task-496 (128 turns, 3.66 M) and task-499, grants x1/x2 on a fake figure. Q8 not done (default no). Commit f8c6dcf5.

## Questions
Decided 2026-11-11 (owner "ok"): every default below is accepted; values provisional, owner 2026-11-11.
- Q6 Handover threshold: 120k tokens of context, checked only at a subtask boundary (the measured value, -21 % on task-495, one run), for every global with subtasks left whatever its family. Default: 120k.
- Q7 Budget trigger: when a closing finds the executor run over the budget of its level, the executor sets wait-answers with what is done, what remains and an estimate, and continues only on the owner word; "continue" grants one more budget of the same size. Budget figures stay those of verification-levels. Default: as written.
- Q8 Also rebuild the other exp/crew-tools items of rapport.md section 7 (lint.js --quiet for the pre-commit hook, experiment mode Saved/experiment.flag)? They were not in the accepted proposal. Default: no.

## Subtasks
1. task-502-01-handover-and-budget (vault/reference/tasks/task-502-executor-handover-budget/task-502-01-handover-and-budget.md) — doc: Rebuild the context measure and the HANDOVER line, the handover section and relay relaunch, and the budget trigger.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-502-01-handover-and-budget]] — step 1, doc.
- [[execution-protocol]] — steps 5.9 handover and 5.10 budget stop written by this task.
- [[agent-executor]] — handover and budget fields written by this task.
- [[verification-levels]] — budget that now stops for the owner.
