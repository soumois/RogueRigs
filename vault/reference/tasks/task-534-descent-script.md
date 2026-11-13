---
id: task-534-descent-script
description: "Global task: a script prints in one call the Children glosses of an index and its sub-indexes with paths; _navigation authorises it as descent."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Script de descente : gloses d'un index et de ses sous-index en un appel

- request: Relay 2026-11-13 (famille : comportement). Demande : "est-ce que tu pourrais voir s'il y a des optimisations de lecture pour le planneur qui semble obvious ( tout en gardant les régles du vault ) ? ( ou utilisation d'outil )". La session principale lui a montré quatre optimisations mesurées sur les trois derniers passages du planner (lint.js qui imprime 74 warnings permanents ; numéro et ancre de tâche qui demandent de lister vault/reference/tasks ; descente gdd en 2 à 3 allers-retours ; pas de ligne d'usage de task-status.js), en recommandant 1, 2 et 4 et en demandant son accord pour 3 qui touche la règle de navigation. Réponse : "Les 4". Pas de capture (outillage, rien de visible en jeu). Point 3 accepté, y compris la modification de la règle de navigation : un script qui imprime en un appel les gloses `## Children` d'un index et de ses sous-index, avec les chemins ; cela reste une descente par gloses, pas une recherche, et une ligne de vault/_navigation.md l'autorise comme pour path-of.js. Owner answer 2026-11-13 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q1, Q2 accepted.
- goal: One call on an index shows the planner every child and grandchild gloss with its path, so it can open the right node directly, while lookup stays a descent by glosses and never a search.
- acceptance: A1 `node vault/scripts/descend.js gdd-index` prints the ## Children entries of gdd-index and, under each child that is itself an index, that child's ## Children entries, each line as id, path and gloss (for example weapons-index then burst-rifle reachable from that one output); A2 it takes only an exact id (or several) as start, prints no node body, no Relations and nothing outside the descent tree, and has no text or topic search parameter; A3 an unknown id exits 1 with a short message, like path-of.js; A4 output for gdd-index stays short (stated size in the report, well under one node body per child); A5 vault/_navigation.md has one line in "Descent is the only lookup method" authorising the script as descent, with the limits of A2; A6 task-spec or agent-planner names it so the planner uses it at step 2.
- gdd_check: Read: root, _navigation, agent-planner, orchestration-protocol, orchestrator-lessons, task-spec, tasks-to-do-index, tech-index, agents-index. No gdd node touched (crew tooling, nothing in game). No open value. No conflict with the vault rules, except point 3 which changes _navigation: owner accepted that change ("Les 4", 2026-11-13). No code read (protocol step 2). Measurements come from the relay analysis of task-531, task-529 and task-520..528 planner transcripts. Owner accepted the _navigation change ("Les 4"). Decision asked: Q1 (name and depth).
- verification_level: standard
- family: comportement
- confirmed_by_owner: yes (2026-11-13, "ok")
- milestone: M2
- next_step: task-534-01-descend-script
- started: 2026-11-13T18:25:34Z
- waiting_for: none
- result: open

## Questions
Decided 2026-11-13 (owner "ok"): every default below is accepted; values provisional, owner 2026-11-13.
- Q1 Nom `descend.js`, profondeur fixe : l'index demandé plus un niveau de sous-index (pas de descente infinie), plusieurs ids possibles en un appel. Default: oui.
- Q2 Niveau de vérification : standard (passages réels sur gdd-index et un id inconnu, lint propre, pas de capture). Default: standard.

## Subtasks
1. task-534-01-descend-script (vault/reference/tasks/task-534-descent-script/task-534-01-descend-script.md) — cpp: Write the vault script that prints the Children glosses of an index and of its child indexes with paths.
2. task-534-02-doc (vault/reference/tasks/task-534-descent-script/task-534-02-doc.md) — doc: Authorise the script in _navigation and name it where the planner reads its tools.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-534-01-descend-script]] — step 1, cpp.
- [[task-534-02-doc]] — step 2, doc.
