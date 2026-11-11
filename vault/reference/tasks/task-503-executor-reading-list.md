---
id: task-503-executor-reading-list
description: "Global task: executor reads runbook and quirks only on a script failure; specialist agent definitions handled as decided; no node merged."
parent: tasks-done-index
kind: task
status: done
---
# Shorter executor reading list and specialist agent definitions

- request: Owner 2026-11-11, verbatim: "ok, prépare la tâche pour mettre ça en place." Earlier in the same conversation, on removing the specialist agent definitions and merging executor reading nodes: "Oui, c'est juste que ça rentrait en conflit avec les régles du vault, ça n'est pas un soucis ?" On ruflo: "Non, ruflo est apparu car j'ai changé de pc et j'avais un plugin activé, mais je ne veux pas ça." "ça" = the main session proposal he accepted: keep the base (planner, one fresh Opus executor per global, style-guide critic, vault as memory) but route each request by family at the entry, picked by the relay from the owner sentence or named by the owner. (1) Réglage (task-490, 491, 474): no planner, one executor on the owner words, a script writes the task node for the trace (a plan costs 0.36 M on average, a light task 0.15 to 0.3 M). (2) Comportement ou système (task-496, 497, 493): planner then executor, plus a handover to a fresh executor at a subtask boundary over 120k context, and a budget that triggers something instead of being indicative. (3) Visuel ou goût (task-499, 461/467/477, 462/466): preview first (mockup or one slice, one capture, owner go), then the full build (task-499: 6.57 M refused at the capture and reverted). (4) Gros morceau (task-484 at 7 M, task-424): the planner splits into several globals, one fresh executor each. Not wanted: specialists per trade, a verifier agent; the only delegation kept is style-guide. Prepare only, launch nothing. Owner answer 2026-11-11 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q9, Q10, Q11 accepted.
- goal: The executor starts with fewer reads without merging vault nodes, and the crew definitions match the single-executor workflow.
- acceptance: A1 agent-executor reads and .claude/agents/executor.md: orchestrator-runbook and orchestrator-script-quirks are read only when a script or command fails; role nodes still on first use of a role. A2 Specialist definitions handled as Q9 decides, with agent-executor roles and CLAUDE.md consistent with it. A3 No vault node merged; each kept node still one concept. A4 Lint clean, committed.
- gdd_check: No gdd node touched: the request changes the dev crew (agents branch). Read: agent-planner, orchestration-protocol, orchestrator-lessons, task-spec, agent-executor, execution-protocol, verification-levels. No code read. Facts from the main session, not re-checked: rapport.md section 7 (2026-11-07) measured the 120k handover at -21 % on task-495 (one run) on branch exp/crew-tools, which no longer exists: rebuild, do not merge. Since then task-496 (standard, budget 90 turns 1.5 M) ran 128 turns, peak 339k, 3.66 M; task-499 cost 6.57 M and was cancelled. Rule nodes orchestration-protocol, execution-protocol and verification-levels (kind: rule) are edited under this owner intent once confirmed. _authoring constraint raised by the owner: no node merge; one concept per node.
- verification_level: light
- confirmed_by_owner: yes (2026-11-11, "ok")
- milestone: M2
- next_step: none
- started: 2026-11-11T21:22:27Z
- waiting_for: none
- executor_tokens: input 26, cache write 40620, cache read 470323, output 8074
- executor_usage: runs 1, turns 13, context start 16k peak 51k, resumes 0, 1 min, units 0.14 M
- planning_tokens: input 5, cache write 12163, cache read 83526, output 4150
- planning_usage: add7b373 "Plan crew workflow routing change" 1/4 (tasks 500, 501, 502, 503), units 0.04 M
- result: Done. A1 agent-executor reads and .claude/agents/executor.md step 1: runbook and quirks only when a script or command fails, role nodes on first use. A2 Q9: .claude/agents cpp, bp, capture, doc, art, 3d, text, level-design deleted; agent-executor roles, CLAUDE.md, runbook delegation and doc-subtask lines, agents-index agent-doc gloss and definition_file of six role nodes aligned. Q10: uncommitted ruvector.db line dropped from .gitignore, ruvector.db deleted. A3 no node merged. A4 lint clean, commit 096a7033. Left as is: README.md, assignments READMEs and .claude/commands/architect.md still link the removed definitions (historical deliverables, not in the request).

## Questions
Decided 2026-11-11 (owner "ok"): every default below is accepted; values provisional, owner 2026-11-11.
- Q9 Specialist agent definitions: delete .claude/agents cpp, bp, capture, doc, art, 3d, text, level-design (keep planner, executor, style-guide); the vault agent-* nodes stay as the role rules the executor reads; the line "they stay defined for a direct owner request" is removed. Default: as written.
- Q10 Ruflo: remove the ruvector.db line from .gitignore and delete ruvector.db at the repo root as part of this task (the plugin itself is disabled by you in Claude Code). Default: yes, in this task.
- Q11 A measured replay of a past task after the four tasks to compare cost. Default: no.

## Subtasks
1. task-503-01-reading-list-and-definitions (vault/reference/tasks/task-503-executor-reading-list/task-503-01-reading-list-and-definitions.md) — doc: Shorten the executor reading list and apply the specialist definitions decision.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-503-01-reading-list-and-definitions]] — step 1, doc.
