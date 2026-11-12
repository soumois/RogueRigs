---
id: task-508-workflow-back-to-old
description: "Global task: Removes gros-morceau split, 120k context handover and active budget from the crew rules; keeps reglage route, preview gate, task-confirm, failure-only runbook, extraProjectSlugs, three agents."
parent: tasks-to-do-index
kind: task
status: todo
---
# Workflow : retour à l'ancien, en gardant routeur réglage, porte visuelle et outils

- request: Message 1 : "Ok, garde le résultat de l'ancien workflow. Ce qu'on peut garder en tout cas, c'est le routeur au départ pour les modifications de valeur( de ne pas faire appel au planneur pour ça ), mais il semble que l'ancien workflow a l'air de mieux fonctionner ( plus rapide et meilleur résultat )." Message 2, en réponse au tableau des points du relais : "Garde le visuel et push sur master, retirer les points à retirer ( garder les images comme l'ancien workflow )". Lecture du relais : garder la porte de prévisualisation visuelle ; mettre sur master l'écran Options de l'ancien workflow avec ses images, puis pousser master ; retirer les trois points marqués "à retirer". Décision du propriétaire par point (relais) : RETIRER le découpage d'un gros morceau en plusieurs globales ; RETIRER le handover de contexte à 120k ; RETIRER le budget actif (handover sur dépassement, arrêt à 5x) ; GARDER la route réglage sans planner, la porte de prévisualisation visuelle, le "ok" pur via task-confirm.js, la lecture runbook/quirks seulement sur échec de script, le suivi de coût sur plusieurs dossiers de transcripts (extraProjectSlugs), seulement trois définitions d'agent (planner, executor, style-guide). Le correctif non commité de .claude/commands/style-loop.md (agent general-purpose dans le rôle texte) est approuvé et part avec cette tâche. Owner answer 2026-11-12 (relayed by the main session, verbatim): "Ok pour tout" = the plan and the defaults of Q1 accepted.
- goal: The crew rules, scripts and Claude files describe and run the old workflow again, minus the three removed points and with the six kept points intact.
- acceptance: A1 a gros morceau is planned as one global with its subtasks: no rule node, agent node, command or CLAUDE.md asks to split it into several globals; A2 no context handover: task-next.js prints no HANDOVER line, alerts.handoverContextTokens is gone, no ## Handover section or relay relaunch on handover remains in the rules, task.md or CLAUDE.md; A3 budget is only recorded: no handover on overrun, no stop at 5x, alerts.budgetStopFactor is gone, nothing blocks on budget; A4 the kept points still work as before: réglage route and task-reglage.js, preview gate (execution-protocol step 5.7, preview_gate), task-confirm.js on a pure ok, runbook/quirks read only on script failure, extraProjectSlugs in cost tracking, only planner, executor and style-guide in .claude/agents; A5 CLAUDE.md, .claude/commands/task.md and .claude/agents agree with the rule nodes; the style-loop.md fix is committed; A6 the touched scripts run without error and lint is clean; tasks 500 to 503 and 507 stay as history, untouched.
- gdd_check: No gdd node touched (crew rules only). Rule nodes concerned: request-families, orchestration-protocol, execution-protocol, agent-planner, agent-executor, verification-levels, task-spec; origin of each point in task-500 to task-503 and task-507. Owner intent explicit, so kind: rule edits are authorised. No open value. Order: runs before task-509 so the options merge is executed under the restored workflow.
- verification_level: light
- family: comportement
- confirmed_by_owner: yes (2026-11-12, "Ok pour tout")
- milestone: M2
- next_step: task-508-01-rules-scripts-claude-files
- started: open
- waiting_for: none
- result: open

## Questions
Decided 2026-11-12 (owner "Ok pour tout"): every default below is accepted; values provisional, owner 2026-11-12.
- Q1 Push : cette tâche (règles) tourne en premier ; master est poussé une seule fois, à la fin de task-509, avec les deux tâches dedans. Défaut : oui, un seul push à la fin.

## Subtasks
1. task-508-01-rules-scripts-claude-files (vault/reference/tasks/task-508-workflow-back-to-old/task-508-01-rules-scripts-claude-files.md) — executor: remove the three points from rule nodes, scripts and Claude files, keep the six others, commit the style-loop fix

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-508-01-rules-scripts-claude-files]] — step 1, executor.
