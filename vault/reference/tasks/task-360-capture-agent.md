---
id: task-360-capture-agent
description: "Global task: New Sonnet capture agent: runs -game captures from a planner checklist with a minimal brief, reports each point seen, not seen or ambiguous."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Capture agent on Sonnet with a checklist

- request: owner, 2026-10-29, in French, verbatim: "Fait de 1 à 3 et est-ce que tu penses qu'un modéle sonnet suffirait pour les captures ? Peut être aussi prévoir un agent pour ça ? Par exemple, on lui donne une liste de chose à vérifier, mais il n'a pas besoin d'avoir tout le contexte. Je suppose qu'actuellement, l'agent BP ou CPP qui vérifie les captures et celui qui a créé ce qu'il doit vérifier". "1 à 3" = the three levers proposed by the main session the same day; this task is the owner's own proposal (a dedicated capture agent with a checklist and a minimal brief). Context relayed by the main session (cost analysis of 2026-10-29, `node vault/scripts/agent-usage.js --from 280` and subagent transcripts, weighted M units): tasks 332-357 cost ~49 M; workers 27.6 (bp 13.3, cpp capture subtasks 4.24 over 18 runs, 3d 3.6, art 2.4, doc 1.24), executor 6.3, planning ~9.7 (~20 %), main sessions ~5.4. Lever 4 (main session per request) is not requested. Facts checked by the main session: today a capture is its own subtask run by a fresh `cpp` agent (Opus), not the one that built the change, but it loads the whole cpp definition (its `reads` list holds five C++ convention nodes) and the task inputs. Main session view: Sonnet is enough to run a capture and judge it against an explicit checklist; ambiguous points are escalated, not decided. Planner answer: agreed, see `gdd_check`. Owner answer 2026-10-29 (relayed by the main session, verbatim): "OK, tout par défaut" = the plan and every default of Q1 to Q12 accepted (owner intent for the rule edits of `_navigation` and `orchestration-protocol` steps 3 and 4 given through Q1, Q2, Q7).
- goal: Visual checks run by a `capture` agent on Sonnet that reads only its subtask, its own node and the capture tool doc, and returns per checklist point seen / not seen / ambiguous with frame or crop paths.
- acceptance: `.claude/agents/capture.md` and `vault/reference/agents/agent-capture.md` exist and agree; agents-index, agent-executor, agent-planner, orchestration-protocol step 3, orchestrator-lessons and agent-cpp (plus `.claude/agents/cpp.md`) route visual checks to `capture` and describe the checklist; lint clean; committed. No capture run (none requested).
- gdd_check: crew tooling, no gdd node involved (Planner, 2026-10-29). Read: agent-cpp (`capture_script`, `capture_rules`, `visual_check`, `reads`), `.claude/agents/cpp.md`, orchestrator-lessons ("Visual checks go to the cpp agent"), orchestration-protocol, agent-executor (`agents` list), task-355-02 (a capture subtask: 42 turns, peak 134k, it also scripted mouse hover with PowerShell). Planner answer to the owner's question: (1) correction: the capture is already run by a fresh cpp instance, not the builder, so the gain is not independence but a smaller brief and a cheaper model; (2) Sonnet suffices for running capture.js and judging explicit points (presence, overlap, clipping, colour, text); it is weaker on 1-2 px judgements, hence crops (task-359) and the ambiguous verdict escalated to the owner; (3) Sonnet lowers price, not units; units fall from dropping the C++ reading list and from task-359 tooling; (4) a capture that needs a code or tool change stops and reports instead of coding (today the cpp capture may patch). Run order: after [[task-359-capture-tooling]]. `orchestration-protocol` is `kind: rule`: the owner's "prévoir un agent pour ça" is taken as intent to edit it, confirmed by Q7. Scheduled tasks 308, 309, 314 have cpp capture subtasks planned before this agent (Q9).
- confirmed_by_owner: yes (2026-10-29, "OK, tout par défaut" to Q1-Q12 and the plan, relayed by the main session)
- milestone: M2
- next_step: task-360-01-capture-agent-definition
- started: 2026-10-29T14:25:15Z
- waiting_for: none
- result: open

## Questions
Decided 2026-10-29 (owner "OK, tout par défaut"): every default below is accepted; values provisional, owner 2026-10-29.
- Q7 Agent `capture`: model sonnet, tools Read, Bash, PowerShell, Glob (no Write/Edit), maxTurns 30, token_ceiling 0.4 M units (provisional); reads only its subtask node, agent-capture and tool-capture; takes over `capture_script`, `capture_rules` (max 5 launches, max 6 images opened, describe only what is seen) and `visual_check` from agent-cpp; must not edit code, assets, config or vault nodes, nor guess a cause; on a needed code or tool change it stops and reports. Stress runs, automation tests and the pacing bot stay with cpp. Default: yes, and this request is the owner intent to edit orchestration-protocol step 3.
- Q8 Checklist: the planner writes it in the capture subtask `done_when` as numbered points C1..Cn taken from the global `acceptance`, each observable on a frame, and lists in `inputs` the look-before frames, the reference images and the debug commands to use when known; the report gives per point seen / not seen / ambiguous with the frame or crop path; ambiguous points go to the owner, the Executor does not decide them. No template change. Default: yes.
- Q9 Tasks 308, 309, 314 (todo, planned before) keep their cpp capture subtasks as written. Default: yes, not re-planned.

## Subtasks
1. task-360-01-capture-agent-definition (vault/reference/tasks/task-360-capture-agent/task-360-01-capture-agent-definition.md) — doc: .claude/agents/capture.md and agent-capture node; capture moved out of agent-cpp; agents-index, executor, planner, protocol step 3, lessons updated.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-360-01-capture-agent-definition]] — step 1, doc.
- [[task-359-capture-tooling]] — runs before; tool-capture is its output.
- [[task-358-planner-cost]] — lever 1, same request.
