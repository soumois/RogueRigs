---
id: task-532-lint-quiet-output
description: "Global task: lint.js prints by default only errors, warnings of modified files and the summary line; --all prints every warning as today."
parent: tasks-done-index
kind: task
status: done
---
# lint.js : sortie courte par défaut

- request: Relay 2026-11-13 (famille : comportement). Demande : "est-ce que tu pourrais voir s'il y a des optimisations de lecture pour le planneur qui semble obvious ( tout en gardant les régles du vault ) ? ( ou utilisation d'outil )". La session principale lui a montré quatre optimisations mesurées sur les trois derniers passages du planner (lint.js qui imprime 74 warnings permanents ; numéro et ancre de tâche qui demandent de lister vault/reference/tasks ; descente gdd en 2 à 3 allers-retours ; pas de ligne d'usage de task-status.js), en recommandant 1, 2 et 4 et en demandant son accord pour 3 qui touche la règle de navigation. Réponse : "Les 4". Pas de capture (outillage, rien de visible en jeu). Point 1 accepté : sortie de lint.js limitée par défaut aux erreurs, aux warnings des fichiers modifiés et à la ligne de résumé, avec un drapeau --all pour le reste. Owner answer 2026-11-13 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q1, Q2 accepted.
- goal: A default lint run shows only what the current change is responsible for, without hiding any error, and the full list stays one flag away.
- acceptance: A1 on a tree with no vault change, `node vault/scripts/lint.js --write` prints the summary line only (today "vault OK — 469 nodes, 74 warning(s)" plus its 74 warning lines); A2 every error is always printed, whatever the files; A3 a warning on a vault file modified, staged or untracked relative to HEAD is printed by default (checked by touching one node so it raises a warning, then restoring it); A4 `--all` prints every warning as before the change; A5 the summary line still counts all warnings and says how many are hidden and that --all shows them; A6 exit codes and the --write behaviour are unchanged, and the scripts that call lint.js (task-create, task-confirm, task-status and others) still work; A7 the vault nodes that describe lint.js usage state the default and --all.
- gdd_check: Read: root, _navigation, agent-planner, orchestration-protocol, orchestrator-lessons, task-spec, tasks-to-do-index, tech-index, agents-index. No gdd node touched (crew tooling, nothing in game). No open value. No conflict with the vault rules, except point 3 which changes _navigation: owner accepted that change ("Les 4", 2026-11-13). No code read (protocol step 2). Measurements come from the relay analysis of task-531, task-529 and task-520..528 planner transcripts. Decision asked: Q1 (what "modified" means).
- verification_level: standard
- family: comportement
- confirmed_by_owner: yes (2026-11-13, "ok")
- milestone: M2
- next_step: none
- started: 2026-11-13T18:19:38Z
- waiting_for: none
- executor_tokens: input 38, cache write 48918, cache read 780764, output 9475
- executor_usage: runs 1, turns 19, context start 16k peak 62k, resumes 0, 2 min, units 0.19 M, USD 0.59
- planning_tokens: input 7, cache write 10621, cache read 103262, output 3169
- planning_usage: a3bd4ed0 "Planifier optimisations lecture planner" 1/4 (tasks 532, 533, 534, 535), units 0.04 M, USD 0.14
- result: Done. orchestrator-runbook (Git section) documents lint.js [--write] [--all]: errors always, default warnings only for vault files differing from HEAD, summary with hidden count, --all for every warning, exit code and --write unchanged; orchestrator-script-quirks notes that committed nodes' warnings are hidden and --all shows them. _authoring (kind rule, not in inputs) left unchanged: its lint line stays true. Lint clean.

## Questions
Decided 2026-11-13 (owner "ok"): every default below is accepted; values provisional, owner 2026-11-13.
- Q1 "Fichiers modifiés" = les fichiers du vault qui diffèrent du dernier commit (modifiés, indexés ou non suivis). Default: oui.
- Q2 Niveau de vérification : standard (contrôles A1 à A7 par des passages réels de lint, pas de capture). Default: standard.

## Subtasks
1. task-532-01-lint-default-output (vault/reference/tasks/task-532-lint-quiet-output/task-532-01-lint-default-output.md) — cpp: Make lint.js print by default only errors, warnings of modified vault files and the summary line, with --all for the full list.
2. task-532-02-doc (vault/reference/tasks/task-532-lint-quiet-output/task-532-02-doc.md) — doc: Update the vault nodes that describe lint.js usage with the default output and --all.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-532-01-lint-default-output]] — step 1, cpp.
- [[task-532-02-doc]] — step 2, doc.
