---
id: task-534-01-descend-script
description: "Cpp: Write the vault script that prints the Children glosses of an index and of its child indexes with paths."
parent: task-534-descent-script
kind: task
status: done
---
# Script de descente : gloses d'un index et de ses sous-index en un appel: cpp

- goal: Write the vault script that prints the Children glosses of an index and of its child indexes with paths.
- agent: cpp
- depends_on: none
- inputs: _navigation (vault/_navigation.md), vault/scripts/path-of.js, gdd-index (vault/reference/gdd/gdd-index.md)
- expected_output: see done_when
- done_when: A1 to A4 of the global hold, shown in the subtask report by real runs on gdd-index, on two ids at once and on an unknown id.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-534
- usage: included in the executor run of task-534
- runs: 1
- milestone: M2
- started: 2026-11-13T18:25:34Z
- waiting_for: none
- result: vault/scripts/descend.js written (commit above). A1: gdd-index prints its 9 children and, under each sub-index, its children (weapons-index then burst-rifle in the same output), each line id, path, gloss. A2: exact ids only (one or several, a wikilink-style bracketed id accepted), no search parameter, no body or Relations printed. A3: unknown id prints 'descend: unknown id <id>' and exits 1; a node without ## Children exits 1 too. A4: gdd-index output 62 lines, 7.9 KB, against 208 KB for the bodies of the 61 nodes it lists. Runs checked: gdd-index, weapons-index + [[tech-index]] in one call, no-such-id, burst-rifle.

## Relations
- [[task-534-descent-script]] — parent global task.
