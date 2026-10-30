---
id: task-385-text-caption-lines
description: "Global task: screen_texts.json lines may hold a short caption ending with a colon (1 to 4 words); validator and schema node changed."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Text validator: caption lines in screen_texts

- request: Owner, 2026-10-30, verbatim (relayed by the main session): "Assouplir la règle", answering the block of task-383-01 (commit b72069c7): `Tools/content/validate_text.js` and the node text-content-files require 3 to 12 words per entry of `lines` in screen_texts.json, and the decided label "Total Ore:" (task-383 Q2) has 2 words: `intermission_recap: lines[0] has 2 words, allowed 3 to 12`. The owner chose option 1: relax the minimum for `lines`, keep the label "Total Ore:". One logical point (the rule), one global; task-383-01 waits for it. Narrowest reading proposed (Q1): only `lines` entries that end with ":" are captions with the labels limit; every other line keeps 3 to 12 words. Not in scope: the other files (upgrade, weapon, barks, alerts), other slots, the log rule "highest-loop output equals the item title" (unchanged: the second problem of task-383-01 is the text agent writing the title as output, as earlier logs do, noted in task-383-01), new self-test cases (no unrequested test). Owner answer 2026-10-30 (relayed by the main session, verbatim): "Ok, par défaut" = the plan and the defaults of Q1 accepted.
- goal: validate_text.js accepts in screen_texts.json a `lines` entry that ends with ":" with 1 to 4 words, keeps 3 to 12 words for every other line, and text-content-files states it.
- acceptance: validate_text.js: in screen_texts.json, a `lines` entry whose trimmed text ends with ":" is checked against 1 to 4 words, every other `lines` entry against 3 to 12 as before; all other files and slots unchanged; error message names the limit used; `--selftest` passes unchanged; the current screen_texts.json with its current log still validates; a scratch copy of screen_texts.json whose intermission_recap lines is ["Total Ore:"] passes the word-count check (not committed); text-content-files row `lines` says the rule with "owner 2026-10-30, task-385"; lint clean; committed by path.
- gdd_check: Read 2026-10-30: [[text-content-files]] (kind doc, child of text-content-schema; screen_texts.json `lines`: "each 3 to 12 words", word limits "provisional (task-209-03, not owner-set)"; `labels` 1 to 4 words; log: highest-loop output equals the item title), `Tools/content/validate_text.js` (FILES.screen_texts.fields.lines {min 3, max 12, array}; checkText applies one limit per slot; --selftest runs Tools/content/tests/, which holds no screen_texts case). Current screen_texts.json lines: intermission_recap two "Ore secured ..." lines (removed by task-383-01) and loading_level three 3-word lines; none ends with ":", so nothing else changes. No kind: rule node is edited. Conflicts: none. Open values: none. Scope M2 tooling.
- confirmed_by_owner: yes (2026-10-30, "Ok, par défaut")
- milestone: M2
- next_step: task-385-01-validator-caption-lines
- started: 2026-10-30T05:17:52Z
- waiting_for: none
- result: open

## Questions
Decided 2026-10-30 (owner "Ok, par défaut"): every default below is accepted; values provisional, owner 2026-10-30.
- Q1 Formulation de la règle assouplie : dans screen_texts.json, une entrée de `lines` qui finit par « : » est un libellé suivi d'une valeur affichée à côté, et suit la limite des labels, 1 à 4 mots ; toutes les autres lignes gardent 3 à 12 mots ; rien ne change pour les autres fichiers de textes ni pour les autres champs. Les lignes actuelles (« Loading the map »…) ne finissent pas par « : », donc rien ne change pour elles. Autre choix, plus large : minimum 2 mots pour toutes les lignes de screen_texts.json. Default: l'exception pour un libellé qui finit par « : ».

## Subtasks
1. task-385-01-validator-caption-lines (vault/reference/tasks/task-385-text-caption-lines/task-385-01-validator-caption-lines.md) — cpp: validate_text.js: a screen_texts lines entry ending with ":" checked against 1 to 4 words, others unchanged
2. task-385-02-doc-caption-lines (vault/reference/tasks/task-385-text-caption-lines/task-385-02-doc-caption-lines.md) — doc: text-content-files: lines rule with the caption exception

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-385-01-validator-caption-lines]] — step 1, cpp.
- [[task-385-02-doc-caption-lines]] — step 2, doc.
- [[text-content-files]] — the schema node whose lines rule changes.
- [[task-383-level-end-total-ore]] — the task blocked by the rule; its subtask 01 waits for this one.
