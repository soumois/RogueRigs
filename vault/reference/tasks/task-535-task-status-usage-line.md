---
id: task-535-task-status-usage-line
description: "Global task: task-spec gets one line with the exact task-status.js syntax, e.g. back to wait-answers with waiting_for."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# task-spec : ligne d'usage de task-status.js

- request: Relay 2026-11-13 (famille : comportement). Demande : "est-ce que tu pourrais voir s'il y a des optimisations de lecture pour le planneur qui semble obvious ( tout en gardant les régles du vault ) ? ( ou utilisation d'outil )". La session principale lui a montré quatre optimisations mesurées sur les trois derniers passages du planner (lint.js qui imprime 74 warnings permanents ; numéro et ancre de tâche qui demandent de lister vault/reference/tasks ; descente gdd en 2 à 3 allers-retours ; pas de ligne d'usage de task-status.js), en recommandant 1, 2 et 4 et en demandant son accord pour 3 qui touche la règle de navigation. Réponse : "Les 4". Pas de capture (outillage, rien de visible en jeu). Point 4 accepté : une ligne de syntaxe exacte de task-status.js dans task-spec.md (forme qui a marché : `node vault/scripts/task-status.js <task-file>.md status=wait-answers "waiting_for=..."`).
- goal: The planner finds in task-spec the exact task-status.js call it needs and gets it right first time.
- acceptance: A1 task-spec has one line giving the exact task-status.js syntax (file argument, status=..., quoted key=value fields) with the wait-answers plus waiting_for example; A2 the syntax is checked against the script's actual behaviour, not copied blindly from the relay; A3 the existing "Do not repeat those transitions with task-status.js" rule stays; A4 lint clean.
- gdd_check: Read: root, _navigation, agent-planner, orchestration-protocol, orchestrator-lessons, task-spec, tasks-to-do-index, tech-index, agents-index. No gdd node touched (crew tooling, nothing in game). No open value. No conflict with the vault rules, except point 3 which changes _navigation: owner accepted that change ("Les 4", 2026-11-13). No code read (protocol step 2). Measurements come from the relay analysis of task-531, task-529 and task-520..528 planner transcripts.
- verification_level: light
- family: comportement
- confirmed_by_owner: no (plan and questions handed back 2026-11-13)
- milestone: M2
- next_step: task-535-01-usage-line
- started: open
- waiting_for: owner answer to Q1
- result: open

## Questions
Asked 2026-11-13, each with a default; open until the owner answers.
- Q1 Niveau de vérification : léger (une ligne de doc vérifiée contre le script, lint propre). Default: léger.

## Subtasks
1. task-535-01-usage-line (vault/reference/tasks/task-535-task-status-usage-line/task-535-01-usage-line.md) — doc: Add the exact task-status.js usage line to task-spec.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-535-01-usage-line]] — step 1, doc.
