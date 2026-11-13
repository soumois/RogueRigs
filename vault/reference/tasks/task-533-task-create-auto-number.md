---
id: task-533-task-create-auto-number
description: "Global task: task-create.js computes the task number and index anchor itself, so the planner no longer lists vault/reference/tasks before creating."
parent: tasks-to-do-index
kind: task
status: todo
---
# task-create.js : numéro et ancre automatiques

- request: Relay 2026-11-13 (famille : comportement). Demande : "est-ce que tu pourrais voir s'il y a des optimisations de lecture pour le planneur qui semble obvious ( tout en gardant les régles du vault ) ? ( ou utilisation d'outil )". La session principale lui a montré quatre optimisations mesurées sur les trois derniers passages du planner (lint.js qui imprime 74 warnings permanents ; numéro et ancre de tâche qui demandent de lister vault/reference/tasks ; descente gdd en 2 à 3 allers-retours ; pas de ligne d'usage de task-status.js), en recommandant 1, 2 et 4 et en demandant son accord pour 3 qui touche la règle de navigation. Réponse : "Les 4". Pas de capture (outillage, rien de visible en jeu). Point 2 accepté : task-create.js attribue num lui-même (max + 1) et prend l'ancre dans l'index, ce qui retire un cycle et la règle "lister vault/reference/tasks" de task-spec. Owner answer 2026-11-13 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q1, Q2 accepted.
- goal: A planner writes a spec without num and without anchor and task-create.js creates correctly numbered, correctly indexed tasks, with no directory listing needed.
- acceptance: A1 a spec with no num on any global gets consecutive numbers starting at the highest existing task number + 1, in the order of the globals; A2 with no anchor (neither in the spec nor on the command line) the script places the new lines in the index exactly where a correct manual anchor would put them, including when tasks-to-do-index has no task line; A3 an explicit num or anchor keeps working as today (num refused if already taken); A4 the printed output names the ids created so the planner can use them without listing; A5 task-reglage.js and task-confirm.js, which share the numbering or the index, still work; A6 task-spec no longer asks to list vault/reference/tasks and shows num and anchor as optional; A7 checked by a dry or scratch run that leaves no stray task node or index line in the vault.
- gdd_check: Read: root, _navigation, agent-planner, orchestration-protocol, orchestrator-lessons, task-spec, tasks-to-do-index, tech-index, agents-index. No gdd node touched (crew tooling, nothing in game). No open value. No conflict with the vault rules, except point 3 which changes _navigation: owner accepted that change ("Les 4", 2026-11-13). No code read (protocol step 2). Measurements come from the relay analysis of task-531, task-529 and task-520..528 planner transcripts. Decision asked: Q1 (explicit num and anchor kept as overrides).
- verification_level: standard
- family: comportement
- confirmed_by_owner: yes (2026-11-13, "ok")
- milestone: M2
- next_step: task-533-01-auto-number-anchor
- started: open
- waiting_for: none
- result: open

## Questions
Decided 2026-11-13 (owner "ok"): every default below is accepted; values provisional, owner 2026-11-13.
- Q1 num et anchor deviennent facultatifs : omis, le script les calcule ; donnés, ils restent acceptés comme aujourd'hui (num refusé s'il est déjà pris). Default: oui.
- Q2 Niveau de vérification : standard (création réelle vérifiée puis annulée, task-reglage et task-confirm non cassés, pas de capture). Default: standard.

## Subtasks
1. task-533-01-auto-number-anchor (vault/reference/tasks/task-533-task-create-auto-number/task-533-01-auto-number-anchor.md) — cpp: Make task-create.js compute the next task number and the index anchor when the spec omits them.
2. task-533-02-doc (vault/reference/tasks/task-533-task-create-auto-number/task-533-02-doc.md) — doc: Update task-spec and the runbook: num and anchor optional, no listing of vault/reference/tasks.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-533-01-auto-number-anchor]] — step 1, cpp.
- [[task-533-02-doc]] — step 2, doc.
