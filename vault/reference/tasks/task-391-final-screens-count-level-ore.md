---
id: task-391-final-screens-count-level-ore
description: "Global task: The run-won end screen ore line shows the run total (same as the level 4 Total Ore), not a smaller number; the stats screen total is checked too."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Run-won end screen shows the run ore total

- request: Owner, 2026-10-30, after testing the packaged build (Saved/Packaged/Windows/RogueRigs.exe), relayed verbatim by the main session (French), four points: "La lueur du coffre bonus reste entre les niveaux" / "Les particules de robot doivent aussi être retirées entre les niveaux (je pouvais voir des particules des robots tués au précédent niveau car j'ai passé rapidement à l'autre niveau)" / "Le dernier niveau doit ramener vers le main menu" / "Pour le dernier écran pour les ore sécurisés (ça n'est pas un autre mot que ore ?), il ne comptait pas ceux du level en cours." A screenshot of the arena was attached (aim chevrons visible, no issue there). This task is point 4 only: "Pour le dernier écran pour les ore sécurisés (ça n'est pas un autre mot que ore ?), il ne comptait pas ceux du level en cours." Evidence (owner, relayed 2026-10-30): he had just seen the level 4 stats screen ("CLAIM EMPTIED", "Total Ore: 168", button "Cash out"); the next screen ("CLAIM STRIPPED", button "Play again") showed "Ore secured: 33". The HUD in that level showed 43 and the objective "Clear the marked rocks (0/33)", so 33 may be the level ore, the balance after purchases, or even the objective count: to check. Note (Planner): "CLAIM STRIPPED" is the run-won end screen (screen_texts end_run_won); the run-lost screen has no ore line (EndOreRow collapsed on failure). Fix: the end screen shows the run total, the same number as Total Ore (168 here); the stats screen Total Ore is checked to include the level just extracted (Q5, Q6 defaults kept). The word question became task-392. Owner answers 2026-10-30 (relayed by the main session, translated): Q1 NO capture subtask on any task (388 to 393): the owner tests by himself; Q2, Q3, Q4 defaults accepted (389 after 388; "Main Menu" button on the run-won screen, loss screen keeps "Play again"; menu opens directly, same behaviour in all modes); the main-menu and run-structure GDD edits approved (applied 2026-10-30); Q5, Q6 defaults stay (both screens, same number as Total Ore); Q7 changed: every on-screen "Ore" becomes "Duskite", its own task (task-392); task-391 evidence added; new point: the miner looks over-lit (task-393).
- goal: After the level 4 extraction, the run-won end screen ore line equals the level 4 stats screen Total Ore (ore secured since the run start including the last extraction, purchases not subtracted); every stats screen Total Ore includes the level just extracted.
- acceptance: cpp: a -game run with known ore per level reproduces the wrong end screen value and the cause is written in the change summary (what 33 was); recap totals include the last extraction; a BlueprintPure getter of the run total for the end screen; bp: WBP_AlertObjectives run-won ore line reads that getter; build passes; compiled with warnings as errors; log values as in done_when; docs updated; lint clean; committed by path; owner checks on screen himself.
- gdd_check: Read 2026-10-30: [[intermission]] (provisional, task-383 Q1: "Total Ore" = ore secured since the run start, purchases not subtracted): no conflict, the defect is in the code. [[ore-economy]] (approved): no conflict. [[run-structure]]: run-won ore line = the stats screen total, written 2026-10-30 (Q6). Tech read: [[sys-run-transition]] (recap RunSecuredOre, OreSecuredThisLevel; level 4 uses GetLevelSecuredOre), [[sys-run]] (RecordIntermissionOreSpent lowers the baselines), [[sys-objectives-ore-counter]] (EndRun(true) secures the carried ore before HandleLevelExtracted), [[wbp-alert-objectives]] (run-won line "Ore secured: " + GetSecuredOre(RunFlow), the bank balance, purchases subtracted), [[wbp-intermission]] (Total Ore from the recap RunSecuredOre). Open values: none. Scope M2. Runs after task-390 (same widget); task-392 then renames the label. No capture subtask (owner Q1, 2026-10-30): the owner checks the result himself in the packaged build. Repo state 2026-10-30: RRPlayerController input fix committed by the owner (091bf23c); Config/DefaultGame.ini (DirectoriesToAlwaysCook) and Config/DefaultEditor.ini have uncommitted owner changes: leave them untouched and never commit them with this task (commit by path).
- confirmed_by_owner: yes (2026-10-30, owner answers relayed by the main session: Q1 no capture, Q2 to Q6 defaults, Q7 changed into task-392)
- milestone: M2
- next_step: task-391-01-level-ore-in-recap
- started: 2026-10-30T21:21:26Z
- waiting_for: none
- result: open

## Questions
Decided 2026-10-30 (owner answers relayed by the main session); values provisional, owner 2026-10-30.
- Q5 Les deux écrans (Total Ore de l'écran de stats et ligne de l'écran de fin) sont vérifiés et corrigés. Answer: oui (défaut), avec la preuve « Total Ore: 168 » puis « Ore secured: 33 ».
- Q6 La ligne de l'écran de fin montre le même nombre que Total Ore (achats non retirés). Answer: oui (défaut).
- Q7 Le mot « Ore ». Answer: remplacer « Ore » par « Duskite » partout à l'écran, tâche à part : task-392.

## Subtasks
1. task-391-01-level-ore-in-recap (vault/reference/tasks/task-391-final-screens-count-level-ore/task-391-01-level-ore-in-recap.md) — cpp: find what the run-won end screen ore value counts, fix the recap totals, pure getter of the run total
2. task-391-02-run-won-ore-line (vault/reference/tasks/task-391-final-screens-count-level-ore/task-391-02-run-won-ore-line.md) — bp: WBP_AlertObjectives: run-won ore line reads the run total getter
3. task-391-03-doc-level-ore (vault/reference/tasks/task-391-final-screens-count-level-ore/task-391-03-doc-level-ore.md) — doc: update the run, transition, ore counter and end screen docs with the corrected ore count

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-391-01-level-ore-in-recap]] — step 1, cpp.
- [[task-391-02-run-won-ore-line]] — step 2, bp.
- [[task-391-03-doc-level-ore]] — step 3, doc.
- [[intermission]] — the Total Ore definition the screens follow.
- [[task-383-level-end-total-ore]] — the task that made the single Total Ore line.
- [[task-390-run-won-back-to-main-menu]] — previous task, same end screen widget.
- [[task-392-duskite-on-screen]] — next task, renames the label on the same widget.
