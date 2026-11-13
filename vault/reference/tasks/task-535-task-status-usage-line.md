---
id: task-535-task-status-usage-line
description: "Global task: task-spec gets one line with the exact task-status.js syntax, e.g. back to wait-answers with waiting_for."
parent: tasks-done-index
kind: task
status: done
---
# task-spec : ligne d'usage de task-status.js

- request: Relay 2026-11-13 (famille : comportement). Demande : "est-ce que tu pourrais voir s'il y a des optimisations de lecture pour le planneur qui semble obvious ( tout en gardant les régles du vault ) ? ( ou utilisation d'outil )". La session principale lui a montré quatre optimisations mesurées sur les trois derniers passages du planner (lint.js qui imprime 74 warnings permanents ; numéro et ancre de tâche qui demandent de lister vault/reference/tasks ; descente gdd en 2 à 3 allers-retours ; pas de ligne d'usage de task-status.js), en recommandant 1, 2 et 4 et en demandant son accord pour 3 qui touche la règle de navigation. Réponse : "Les 4". Pas de capture (outillage, rien de visible en jeu). Point 4 accepté : une ligne de syntaxe exacte de task-status.js dans task-spec.md (forme qui a marché : `node vault/scripts/task-status.js <task-file>.md status=wait-answers "waiting_for=..."`). Owner answer 2026-11-13 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q1 accepted.
- goal: The planner finds in task-spec the exact task-status.js call it needs and gets it right first time.
- acceptance: A1 task-spec has one line giving the exact task-status.js syntax (file argument, status=..., quoted key=value fields) with the wait-answers plus waiting_for example; A2 the syntax is checked against the script's actual behaviour, not copied blindly from the relay; A3 the existing "Do not repeat those transitions with task-status.js" rule stays; A4 lint clean.
- gdd_check: Read: root, _navigation, agent-planner, orchestration-protocol, orchestrator-lessons, task-spec, tasks-to-do-index, tech-index, agents-index. No gdd node touched (crew tooling, nothing in game). No open value. No conflict with the vault rules, except point 3 which changes _navigation: owner accepted that change ("Les 4", 2026-11-13). No code read (protocol step 2). Measurements come from the relay analysis of task-531, task-529 and task-520..528 planner transcripts.
- verification_level: light
- family: comportement
- confirmed_by_owner: yes (2026-11-13, "ok")
- milestone: M2
- next_step: none
- started: 2026-11-13T18:27:41Z
- waiting_for: none
- executor_tokens: input 16, cache write 25836, cache read 224350, output 3726
- executor_usage: runs 1, turns 8, context start 16k peak 38k, resumes 0, 1 min, units 0.07 M, USD 0.25
- planning_tokens: input 7, cache write 10621, cache read 103262, output 3169
- planning_usage: a3bd4ed0 "Planifier optimisations lecture planner" 1/4 (tasks 532, 533, 534, 535), units 0.04 M, USD 0.14
- result: Done. task-spec Status section has one line with the exact task-status.js syntax and the wait-answers plus waiting_for example (commit 8f7787f2). Checked on a scratch copy of the script: global and subtask wait-answers write waiting_for and the index status; missing waiting_for, waiting_for without status=wait-answers, a vault/... path and an unknown field are refused; status=in-progress resets waiting_for to none. The rule Do not repeat those transitions stays. Lint clean.

## Questions
Decided 2026-11-13 (owner "ok"): every default below is accepted; values provisional, owner 2026-11-13.
- Q1 Niveau de vérification : léger (une ligne de doc vérifiée contre le script, lint propre). Default: léger.

## Subtasks
1. task-535-01-usage-line (vault/reference/tasks/task-535-task-status-usage-line/task-535-01-usage-line.md) — doc: Add the exact task-status.js usage line to task-spec.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-535-01-usage-line]] — step 1, doc.
