---
id: task-538-task-next-close-guard
description: "Global task: task-next.js runs every refusal check before its first write; a rerun after a late failure finishes the close without a second cost row."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# task-next.js refuse avant d'écrire

- request: Point signalé par la session principale après les tâches 532 à 535 : "Clôture de sous-tâche : trois executors sur quatre ont dû finir une clôture à la main. Deux fois, `task-next.js` appelé sans les arguments de la sous-tâche suivante a enregistré le coût puis refusé de clore. Une fois, un lien entre doubles crochets dans le texte de résultat a fait échouer le commit. C'est le même genre de perte que les quatre points." Question : "Je lance une tâche pour corriger le piège de clôture de task-next.js ?" Réponse du propriétaire (2026-11-13) : "Oui, lance". Faits des rapports : task-532 et task-533 sous-tâche 01 sur 2, appel sans les arguments next-global / next-subtask : la ligne de coût est écrite et la sous-tâche passée done, puis la clôture du global est refusée (sous-tâche 02 ouverte) ; fini à la main (commits 415ca094, aa5be272), sans ligne de coût en double. task-534 sous-tâche 01 : bons arguments, mais un lien double-crochet vers un id inexistant dans le texte de résultat fait échouer lint puis le commit, après coût écrit et sous-tâche 02 passée in-progress ; fini à la main (6f39d441). task-535 (une seule sous-tâche, prévenu) sans souci. Owner answer 2026-11-13 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q1, Q2, Q3 accepted.
- goal: A task-next.js call that is going to be refused or fail changes nothing: every check that can refuse runs before the first write (cost row, status, next_step, index, commit). A failure that can only appear after the writes (commit, lint of another file) leaves a state that the same command, rerun, finishes without a second cost row or a double status change. Correct calls behave as today.
- acceptance: A1 On a throwaway global with two subtasks, task-next.js on subtask 01 without the next-global and next-subtask arguments exits non-zero, says which arguments are missing and prints the command form with the next open subtask of the global, and leaves the cost log, the task nodes, the indexes and git exactly as before (status and diff identical). A2 Same throwaway task, correct arguments, result text holding a double-bracket link to a non-existent id: refused before any write, the message names the link, nothing changed. A3 Every other refusal task-next.js already makes (for example closing a global with an open subtask) now happens before its first write. A4 A failure forced after the writes (for example the commit refused): rerunning the same command completes the close (subtask done, next subtask in-progress or global closed, one commit) and the cost log holds exactly one row for that subtask. A5 A correct call on the throwaway task closes subtask 01, starts subtask 02 and commits as today; closing the last subtask closes the global as today. A6 The throwaway task and its cost rows leave no trace in the repo or the indexes. A7 The crew nodes that describe closing a subtask state the new behaviour; the trap entries about missing arguments and the double-bracket link in the result text are rewritten or removed.
- gdd_check: Crew tooling only, no gdd node touched, nothing visible in game, no capture. Nodes read: root, _navigation, agent-planner, orchestration-protocol, orchestrator-lessons, task-spec, tasks-to-do-index; agents-index printed by descend.js to pick the doc nodes. No conflict, no open value, no milestone scope issue. Code not read (no question depended on it). Decisions are the three questions with defaults (refuse rather than guess the next subtask; refuse a dead link rather than rewrite it; rerun finishes a late failure). Unrelated: task-536 uncommitted changes and task-537 wait-answers; commit by explicit path.
- verification_level: standard
- family: comportement
- confirmed_by_owner: yes (2026-11-13, "ok")
- milestone: M2
- next_step: task-538-01-guard
- started: 2026-11-13T19:08:23Z
- waiting_for: none
- result: open

## Questions
Decided 2026-11-13 (owner "ok"): every default below is accepted; values provisional, owner 2026-11-13.
- Q1 Appel sans les arguments de la sous-tâche suivante alors qu'une sous-tâche du global reste ouverte : le script refuse avant toute écriture et affiche la commande complète avec la prochaine sous-tâche ouverte, plutôt que de la deviner et l'enchaîner seul. Default: refuser et afficher la commande (pas de devinette).
- Q2 Lien entre doubles crochets vers un id inexistant dans le texte de résultat : le script refuse avant toute écriture en nommant le lien, plutôt que de le convertir lui-même en texte simple. Default: refuser en nommant le lien.
- Q3 Échec qui ne peut apparaître qu'après les écritures (commit refusé, lint sur un autre fichier) : relancer la même commande termine la clôture, sans deuxième ligne de coût ni double changement de statut, plutôt qu'un retour en arrière automatique. Default: la relance termine.

## Subtasks
1. task-538-01-guard (vault/reference/tasks/task-538-task-next-close-guard/task-538-01-guard.md) — cpp: task-next.js runs every refusal check before its first write and resumes cleanly on rerun after a late failure
2. task-538-02-doc (vault/reference/tasks/task-538-task-next-close-guard/task-538-02-doc.md) — doc: crew nodes describe the new closing behaviour of task-next.js

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-538-01-guard]] — step 1, cpp.
- [[task-538-02-doc]] — step 2, doc.
