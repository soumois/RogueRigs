---
id: task-391-final-screens-count-level-ore
description: "Global task: The stats screen Total Ore and the run-won Ore secured line include the ore secured by the extraction just made."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# End screens count the ore of the level just extracted

- request: Owner, 2026-10-30, after testing the packaged build (Saved/Packaged/Windows/RogueRigs.exe), relayed verbatim by the main session (French), four points: "La lueur du coffre bonus reste entre les niveaux" / "Les particules de robot doivent aussi être retirées entre les niveaux (je pouvais voir des particules des robots tués au précédent niveau car j'ai passé rapidement à l'autre niveau)" / "Le dernier niveau doit ramener vers le main menu" / "Pour le dernier écran pour les ore sécurisés (ça n'est pas un autre mot que ore ?), il ne comptait pas ceux du level en cours." A screenshot of the arena was attached (aim chevrons visible, no issue there). This task is point 4 only: "Pour le dernier écran pour les ore sécurisés (ça n'est pas un autre mot que ore ?), il ne comptait pas ceux du level en cours." Reading (Planner): the ore number shown at the end ("Total Ore" on the stats screen, "Ore secured: N" on the run-won end screen) misses the ore of the level just finished. Which screen exactly is asked in Q5; what the run-won line counts in Q6. The word question: the GDD names the ore Duskite (provisional, [[ore-blocks]]); the screens say "Ore"; whether to rename on screen is asked in Q7 (default: no change, answer only). Not in scope: other texts, the shop counter.
- goal: After every extraction (levels 1 to 4), the stats screen Total Ore equals the ore secured since the run start including this extraction (purchases not subtracted), and the run-won end screen ore line shows the same number.
- acceptance: cpp: cause found by a -game run with known ore per level (rr.Cheat.GrantOre) and written in the change summary; recap RunSecuredOre (and the level 4 path GetLevelSecuredOre) include the ore secured by the extraction just made; a BlueprintPure getter of that run total available to the end screen; bp: WBP_AlertObjectives run-won ore line reads that getter (Q6); build passes; compiled with warnings as errors; capture C1 to C4 seen; docs updated; gdd run-structure updated (edit applied on confirmation); lint clean; committed by path.
- gdd_check: Read 2026-10-30: [[intermission]] (provisional, task-383 Q1: one line "Total Ore" = ore secured since the run start, purchases not subtracted): the screen must include the level just extracted; no conflict, the defect is in the code. [[ore-economy]] (approved: ore secured by extraction, kept across levels, reset at run end): no conflict. [[run-structure]]: the run-won screen ore line is not specified; set by Q6 (edit). [[gdd-glossary]] "Ore / special ore"; [[ore-blocks]] "The ore is called Duskite (provisional name)": the in-game word is "Ore" ("Total Ore:", "Ore secured: N"), the GDD name of the only ore is Duskite (Q7). Tech read: [[sys-run-transition]] (recap RunSecuredOre, OreSecuredThisLevel; level 4 uses GetLevelSecuredOre), [[sys-run]] (RecordIntermissionOreSpent lowers the baselines), [[sys-objectives-ore-counter]] (EndRun(true) secures the carried ore before HandleLevelExtracted), [[wbp-alert-objectives]] (run-won line "Ore secured: " + GetSecuredOre(RunFlow) = bank balance, purchases subtracted), [[wbp-intermission]] (Total Ore from the recap RunSecuredOre). Open values: none after Q5 to Q7. Scope M2. Runs after task-390 (same widget WBP_AlertObjectives). Capture per Q1 of task-388. Repo state 2026-10-30: uncommitted changes not made by this task in Source/RogueRigs/Player/RRPlayerController.cpp/.h (RestoreGameInputMode, packaged input fix), Config/DefaultGame.ini and Config/DefaultEditor.ini: leave them untouched and never commit them with this task (commit by path). task-387 (art, wait-answers) shares no file.
- confirmed_by_owner: no (plan and questions handed back 2026-10-30)
- milestone: M2
- next_step: task-391-01-level-ore-in-recap
- started: open
- waiting_for: owner confirmation of the plan
- result: open

## Questions
Asked 2026-10-30, each with a default; open until the owner answers.
- Q5 Quel écran : le « Total Ore » de l'écran de stats (après chaque niveau, 1 à 4) et la ligne « Ore secured » de l'écran de victoire sont tous deux vérifiés et corrigés pour compter le minerai du niveau qui vient d'être extrait. Default: les deux.
- Q6 La ligne « Ore secured » de l'écran de victoire montre aujourd'hui le solde (ce qui reste après les achats au Spare Parts). Elle montre le même nombre que « Total Ore » (tout le minerai sécurisé depuis le début du run, achats non retirés). Autre choix : garder le solde. Default: même nombre que Total Ore.
- Q7 Le mot : le GDD nomme le minerai « Duskite » (nom provisoire, le seul minerai pour l'instant) ; les écrans disent « Ore ». Default: aucun changement de texte dans cette tâche (si vous voulez « Duskite » à l'écran, ce sera une tâche à part).

## Subtasks
1. task-391-01-level-ore-in-recap (vault/reference/tasks/task-391-final-screens-count-level-ore/task-391-01-level-ore-in-recap.md) — cpp: find why the end screens miss the ore of the level just extracted and fix the recap; pure getter of the run total
2. task-391-02-run-won-ore-line (vault/reference/tasks/task-391-final-screens-count-level-ore/task-391-02-run-won-ore-line.md) — bp: WBP_AlertObjectives: run-won ore line reads the run total getter
3. task-391-03-ore-screens-capture (vault/reference/tasks/task-391-final-screens-count-level-ore/task-391-03-ore-screens-capture.md) — capture: -game frames: Total Ore on the stats screens and Ore secured on the run-won screen with known ore per level
4. task-391-04-doc-level-ore (vault/reference/tasks/task-391-final-screens-count-level-ore/task-391-04-doc-level-ore.md) — doc: update the run, transition, ore counter and end screen docs with the corrected ore count

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-391-01-level-ore-in-recap]] — step 1, cpp.
- [[task-391-02-run-won-ore-line]] — step 2, bp.
- [[task-391-03-ore-screens-capture]] — step 3, capture.
- [[task-391-04-doc-level-ore]] — step 4, doc.
- [[intermission]] — the Total Ore definition the stats screen must follow.
- [[ore-blocks]] — the Duskite name of the ore.
- [[task-383-level-end-total-ore]] — the task that made the single Total Ore line.
- [[task-390-run-won-back-to-main-menu]] — previous task, same end screen widget.
