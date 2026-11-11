---
id: task-500-request-families
description: "Global task: Crew workflow: the relay classifies a request (réglage, comportement, visuel, gros morceau); réglage skips the planner; gros morceau is split into several globals."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Route each owner request by family at the entry

- request: Owner 2026-11-11, verbatim: "ok, prépare la tâche pour mettre ça en place." Earlier in the same conversation, on removing the specialist agent definitions and merging executor reading nodes: "Oui, c'est juste que ça rentrait en conflit avec les régles du vault, ça n'est pas un soucis ?" On ruflo: "Non, ruflo est apparu car j'ai changé de pc et j'avais un plugin activé, mais je ne veux pas ça." "ça" = the main session proposal he accepted: keep the base (planner, one fresh Opus executor per global, style-guide critic, vault as memory) but route each request by family at the entry, picked by the relay from the owner sentence or named by the owner. (1) Réglage (task-490, 491, 474): no planner, one executor on the owner words, a script writes the task node for the trace (a plan costs 0.36 M on average, a light task 0.15 to 0.3 M). (2) Comportement ou système (task-496, 497, 493): planner then executor, plus a handover to a fresh executor at a subtask boundary over 120k context, and a budget that triggers something instead of being indicative. (3) Visuel ou goût (task-499, 461/467/477, 462/466): preview first (mockup or one slice, one capture, owner go), then the full build (task-499: 6.57 M refused at the capture and reverted). (4) Gros morceau (task-484 at 7 M, task-424): the planner splits into several globals, one fresh executor each. Not wanted: specialists per trade, a verifier agent; the only delegation kept is style-guide. Prepare only, launch nothing. Owner answer 2026-11-11 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q1, Q2, Q3, Q4 accepted.
- goal: Every owner request is routed at the entry to one of four families and follows that family path; a setting change runs without a planner and still leaves a task node with its cost.
- acceptance: A1 .claude/commands/task.md and the CLAUDE.md "Claude-specific" section tell the relay how to pick the family from the owner sentence, the owner words that force one (réglage, comportement, visuel, gros), and to state the chosen family in one line before launching anything. A2 Réglage path: no planner; one fresh executor works from the owner words; a script writes the global node (owner words in request, level light, family recorded) at the moment decided in Q2, so cost recording and closing work as for a planned task; shown by running the script on a sample sentence, the sample not committed. A3 Wrong family: a réglage executor that meets a new asset or system, an open value or a design question stops and reports as decided in Q1, so the relay relaunches through the planner; written in the executor nodes. A4 Gros morceau: orchestration-protocol step 3 and agent-planner say when the planner splits one request into several globals (threshold of Q3), each run in order by its own fresh executor. A5 Comportement and visuel keep the planner path (visuel gets its preview in task-501-visual-preview-gate). A6 Lint clean, committed.
- gdd_check: No gdd node touched: the request changes the dev crew (agents branch). Read: agent-planner, orchestration-protocol, orchestrator-lessons, task-spec, agent-executor, execution-protocol, verification-levels. No code read. Facts from the main session, not re-checked: rapport.md section 7 (2026-11-07) measured the 120k handover at -21 % on task-495 (one run) on branch exp/crew-tools, which no longer exists: rebuild, do not merge. Since then task-496 (standard, budget 90 turns 1.5 M) ran 128 turns, peak 339k, 3.66 M; task-499 cost 6.57 M and was cancelled. Rule nodes orchestration-protocol, execution-protocol and verification-levels (kind: rule) are edited under this owner intent once confirmed. _authoring constraint raised by the owner: no node merge; one concept per node.
- verification_level: standard
- confirmed_by_owner: yes (2026-11-11, "ok")
- milestone: M2
- next_step: task-500-01-routing-and-reglage-path
- started: 2026-11-11T21:12:18Z
- waiting_for: none
- result: open

## Questions
Decided 2026-11-11 (owner "ok"): every default below is accepted; values provisional, owner 2026-11-11.
- Q1 Classification: the relay picks the family from the sentence and states it in one line; the owner can force it with one word. A wrong family: a réglage executor that finds a new asset or system, an open value or a design question stops before editing, reports, and the relay relaunches through the planner. Default: as written.
- Q2 Réglage node: a script writes the global node (one subtask, level light, owner words as request, confirmed by the request itself) at the START of the executor run rather than after, so resume, cost recording and the closing gate keep working if the run stops midway. Default: at the start.
- Q3 Gros morceau: the planner splits into several globals when the plan would exceed about one standard budget (1.5 M) or 5 subtasks; each global ends in a usable state; all are confirmed together and run one after another, one fresh executor each. Default: as written.
- Q4 Sonnet for réglage executors (measured -26 to -30 % units, slightly less thorough). Default: no, Opus stays.

## Subtasks
1. task-500-01-routing-and-reglage-path (vault/reference/tasks/task-500-request-families/task-500-01-routing-and-reglage-path.md) — doc: Write the family routing in the relay instructions, the réglage path with its node script, the wrong-family stop and the gros morceau split rule.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-500-01-routing-and-reglage-path]] — step 1, doc.
