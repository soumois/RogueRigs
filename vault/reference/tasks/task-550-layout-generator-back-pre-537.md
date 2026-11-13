---
id: task-550-layout-generator-back-pre-537
description: "Global task: C++ layout generator back to its pre-537 maps (stale pocket guard kept), task-536 speed-up kept, old hashes back, docs updated."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Générateur de layout : retour aux cartes d'avant task-537 (garde des poches)

- request: Owner words (2026-11-14, verbatim, sent with two crops of the task-539 review sheet showing a wide open floor area with a magenta repair outcrop in its middle): "Sur les nouveaux resultat on a plus souvent des pates de vide comparer a l ancien modele et je preferais avant". Relay reading, to be confirmed by the owner (Q1): he prefers the layouts the generator produced before task-537-layout-stale-pocket-guard (code c324273f, docs c9ade13f, result 64780ad8: the guard of removed hidden pockets is released, maps and reference hash changed, arena_l3 level seed 422704489 -> seed 422704499, hash 86c36f6d -> 34b074db) and wants that behaviour back; "pâtés de vide" = large open floor blobs, the kind the review repair breaks with a rock outcrop (magenta on the task-539 sheet). "More often" is his visual impression, not measured (the before code could not be rebuilt in task-539). task-536-layout-filler-cheaper (code 46003871, output-preserving, before task-537) is assumed to stay. Points the relay could not decide: whether the task-537 instrumentation counters stay, revert versus a param switch, anything beyond going back (cap on open blobs or hidden region size).
- goal: The C++ layout generator produces again, for the same params, brief and seed, exactly the maps it produced before task-537 (the guard of a removed hidden pocket stays as it did), with the task-536 filler speed-up kept, and the generator docs describe that behaviour.
- acceptance: A1 Old maps back: `-carveonly` gives the pre-537 hashes recorded in task-536 (seed 3 d6b4a786, seed 1800 53b2a01a, seed 2900 98a7d029, seed 17 0bef55d5, arena_l3 seed 422704496 01c42e39), and the reference line (level seed 422704489 with arena_l3 params and brief, run twice) accepts seed 422704499 with cells hash 86c36f6d on both runs. A2 The 24 starts of task-492-03 each accept the same seed as in the "before" column of the task-537-01 table (the task-495-02 result), pass the level-layout-schema validator, and a table gives per start accepted seed, seeds tried and search time, with the total search time stated next to the 16.6 s (before 537) and 10.9 s (after 537) of task-537. A3 Timing reported, no target: `-carveonly` filler time and idle iterations on seed 17 and arena_l3 seed 422704496, stated next to the task-536 and task-537 figures. A4 Existing tests pass; tests or expectations that only exist for the guard release are removed or updated, nothing else. The Node script, the committed layouts in Content/RogueRigs/Data/Levels and the params files are unchanged. A5 Docs: sys-layout-generator, layout-generator-recipe and layout-review-rules (including the line added by task-539) say what is true after the change: guard behaviour as before 537, reference hash 86c36f6d, timings, that task-537 was undone by this task on owner request, and that the task-539 verdicts were about maps no longer produced.
- gdd_check: Read: arena-grid (M2 deterministic C++ generator, used in runs; no rule on open area size or pocket guard), verification-levels, task-537-layout-stale-pocket-guard and task-539-ld-review-pocket-guard-maps (named by the relay; their results give the hashes, the 24-start comparison and the timings used in acceptance). No gdd node changes: the guard rule lives in tech nodes (layout-generator-recipe, sys-layout-generator, layout-review-rules), updated by the doc subtask. No conflict, no `open` value needed. In scope M2. No code read by the Planner. Trade-off told to the owner (Q2): task-537 made the search faster (24 starts 16.6 -> 10.9 s, slow seed carve 0.81 -> 0.19 s); going back loses that gain, task-536 kept. Not visible on a screen element (no widget, HUD or asset): no capture subtask; the check is the exact return of the old hashes. Level standard: changed behaviour of a system with changed output, though the check is exact. Step 0: task-543-wall-debris-less-white is in progress (its uncommitted files are not this task's).
- verification_level: standard
- family: comportement
- confirmed_by_owner: no (plan and questions handed back 2026-11-13)
- milestone: M2
- next_step: task-550-01-generator-back
- started: open
- waiting_for: Owner answers to Q1-Q4 (restatement, speed trade-off, mechanism and counters, scope beyond going back)
- result: open

## Questions
Asked 2026-11-13, each with a default; open until the owner answers.
- Q1 Je comprends : les « pâtés de vide » sont les grandes zones de sol ouvert (celles que la réparation casse avec un îlot de roche, en magenta sur la planche), et tu veux que le générateur redonne exactement les cartes d'avant task-537 (mêmes seeds, mêmes cartes qu'avant), sans nouvelle règle. Default: oui, retour exact aux cartes d'avant task-537 ; le contrôle est le retour des anciens hashes (référence 86c36f6d) et des seeds acceptés d'avant sur les 24 départs.
- Q2 Task-537 avait rendu la génération plus rapide (24 départs : 16,6 s -> 10,9 s ; seed lent : 0,8 s -> 0,2 s de creusement). Revenir en arrière perd ce gain ; l'accélération de task-536 reste (le seed lent reste à environ 0,8 s au lieu de 4,9 s). La génération des niveaux tourne en arrière-plan (menu, extracteur). Default: on accepte cette perte, le temps est seulement rapporté.
- Q3 Comment revenir : retour simple à l'ancien comportement, ou garder le nouveau sélectionnable par un paramètre (désactivé par défaut) ? Et les compteurs de mesure ajoutés par task-537 (poches, garde, ligne de log `-carveonly`) ? Default: retour simple sans paramètre ; les compteurs qui mesurent seulement (poches retirées, itérations à vide) restent s'ils ne changent pas les cartes, ceux qui ne décrivent que la libération du garde partent avec elle.
- Q4 Autre chose que le retour en arrière, par exemple une limite sur la taille des zones ouvertes ou des régions cachées (les défauts 2700 et 2800 de task-539), ou une mesure chiffrée de « plus souvent » avant/après ? Default: non, retour en arrière seul ; une limite ou une mesure deviendrait une tâche séparée si tu la demandes.

## Subtasks
1. task-550-01-generator-back (vault/reference/tasks/task-550-layout-generator-back-pre-537/task-550-01-generator-back.md) — cpp: Bring the C++ generator back to its pre-537 maps (task-536 kept), check the old hashes and the 24 starts, report timings
2. task-550-02-doc (vault/reference/tasks/task-550-layout-generator-back-pre-537/task-550-02-doc.md) — doc: Make the generator docs say the pre-537 guard behaviour, the reference hash and timings, and that task-537 was undone

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-550-01-generator-back]] — step 1, cpp.
- [[task-550-02-doc]] — step 2, doc.
