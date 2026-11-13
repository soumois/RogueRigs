---
id: task-550-02-doc
description: "Doc: Make the generator docs say the pre-537 guard behaviour, the reference hash and timings, and that task-537 was undone"
parent: task-550-layout-generator-back-pre-537
kind: task
status: done
---
# Générateur de layout : retour aux cartes d'avant task-537 (garde des poches): doc

- goal: Make the generator docs say the pre-537 guard behaviour, the reference hash and timings, and that task-537 was undone
- agent: doc
- depends_on: task-550-01-generator-back
- inputs: sys-layout-generator (vault/reference/tech/systems/world/sys-grid/sys-layout-generator.md), layout-generator-recipe (vault/reference/tech/pipelines/layout-generator-recipe.md), layout-review-rules (vault/reference/tech/pipelines/layout-review-rules.md), task-539-ld-review-pocket-guard-maps (vault/reference/tasks/task-539-ld-review-pocket-guard-maps.md)
- expected_output: see done_when
- done_when: A5 of the global holds: the three nodes describe the behaviour after this task, reference 86c36f6d with seed 422704499, the timings of subtask 1, a dated line that task-537 was undone on owner request, and the task-539 line says its verdicts concern maps no longer produced; lint clean, committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-550
- usage: included in the executor run of task-550
- runs: 1
- milestone: M2
- started: 2026-11-13T23:58:18Z
- waiting_for: none
- result: Done (doc 2854925e). sys-layout-generator: pocket_guard says the guard of pocket cells back to rock stays for the rest of the carve (as the Node script), task-537 (c324273f) undone 2026-11-14 on owner request by task-550 (cc8a7dc8), stale-guard counter = the guard 537 released; -carveonly line without 'released'; 24 starts same seeds as task-495-02, 16.2 s (10.9 s with the release); filler figures seed 17 1.30/1.19 s, 422704496 0.67/0.60 s next to task-536 and 537; reference 422704499 / 86c36f6d plus the five -carveonly hashes. layout-generator-recipe: pocket guard line rewritten, dated undo. layout-review-rules: task-539 line says its verdicts concern maps no longer produced, new dated line for the undo. Lint clean.

## Relations
- [[task-550-layout-generator-back-pre-537]] — parent global task.
