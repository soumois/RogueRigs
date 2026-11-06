---
id: task-391-final-screens-count-level-ore
description: "Global task: every on-screen ore number (stats Total Ore, shop balance, run-won line, HUD) shows the ore the player holds, the balance."
parent: tasks-done-index
kind: task
status: done
---
# On-screen ore numbers show the balance held

m=>m
- goal: After every extraction (levels 1 to 4), the stats screen Total Ore equals the ore balance the player holds (secured after this extraction, purchases and rerolls subtracted), the same number as the shop balance that follows and as the run-won end screen line; no screen shows ore collected since the run start.
- acceptance: cpp (task-391-04): recap total = bank balance at the stats screen, logs as in its done_when, cd19ba03 secured-since-reset code removed when unused (no dead code), recap field name kept so no widget changes; build passes; no bp change; gdd intermission, ore-economy and run-structure say balance; docs updated (task-391-05); lint clean; committed by path; owner checks on screen himself.
- gdd_check: Read 2026-10-30: [[intermission]] (provisional, task-383 Q1: "Total Ore" = ore secured since the run start, purchases not subtracted): no conflict, the defect is in the code. [[ore-economy]] (approved): no conflict. [[run-structure]]: run-won ore line = the stats screen total, written 2026-10-30 (Q6). Tech read: [[sys-run-transition]] (recap RunSecuredOre, OreSecuredThisLevel; level 4 uses GetLevelSecuredOre), [[sys-run]] (RecordIntermissionOreSpent lowers the baselines), [[sys-objectives-ore-counter]] (EndRun(true) secures the carried ore before HandleLevelExtracted), [[wbp-alert-objectives]] (run-won line "Ore secured: " + GetSecuredOre(RunFlow), the bank balance, purchases subtracted), [[wbp-intermission]] (Total Ore from the recap RunSecuredOre). Open values: none. Scope M2. Runs after task-390 (same widget); task-392 then renames the label. No capture subtask (owner Q1, 2026-10-30): the owner checks the result himself in the packaged build. Repo state 2026-10-30: RRPlayerController input fix committed by the owner (091bf23c); Config/DefaultGame.ini (DirectoriesToAlwaysCook) and Config/DefaultEditor.ini have uncommitted owner changes: leave them untouched and never commit them with this task (commit by path). Re-check 2026-10-30 after the owner spec change: [[intermission]] said "the ore secured since the run start, purchases not subtracted" (task-383 Q1): conflict, changed to the balance; [[run-structure]] (run-won line = stats screen total) wording changed to the balance; [[ore-economy]] gets the rule "every on-screen ore number is the balance held". [[wbp-intermission]]: Total Ore reads the recap field RunSecuredOre by name, so the cpp keeps the field name. No longer touches WBP_AlertObjectives: no order constraint with task-390 or task-392.
- confirmed_by_owner: yes (2026-10-30, owner answers relayed by the main session: Q1 no capture, Q2 to Q6 defaults, Q7 changed into task-392)
- milestone: M2
- next_step: none
- started: 2026-10-30T22:14:00Z
- waiting_for: none
- executor_tokens: input 42, cache write 136615, cache read 475995, output 9280
- executor_usage: runs 2, turns 18, context start 18k peak 43k, resumes 0, 31 min, units 0.26 M
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Done. Seven tech nodes state the balance rule and current code; commit 6045f7c0

## Questions
Decided 2026-10-30 (owner answers relayed by the main session); values provisional, owner 2026-10-30.
- Q5 Les deux écrans (Total Ore de l'écran de stats et ligne de l'écran de fin) sont vérifiés et corrigés. Answer: oui (défaut), avec la preuve « Total Ore: 168 » puis « Ore secured: 33 ».
- Q6 La ligne de l'écran de fin montre le même nombre que Total Ore (achats non retirés). Answer: oui (défaut).
- Q11 (2026-10-30, owner decision after task-391-01, final) Sur chaque écran où il voit le minerai, c'est toujours le minerai qu'il possède (le solde). Replaces Q6: the run-won line and the stats total both show the balance, not the ore collected since the run start.
- Q7 Le mot « Ore ». Answer: remplacer « Ore » par « Duskite » partout à l'écran, tâche à part : task-392.

## Subtasks
1. task-391-01-level-ore-in-recap (vault/reference/tasks/task-391-final-screens-count-level-ore/task-391-01-level-ore-in-recap.md) — cpp: find what the run-won end screen ore value counts, fix the recap totals, pure getter of the run total
2. task-391-02-run-won-ore-line (vault/reference/tasks/task-391-final-screens-count-level-ore/task-391-02-run-won-ore-line.md) — bp: cancelled 2026-10-30 (the end screen already reads the balance)
3. task-391-03-doc-level-ore (vault/reference/tasks/task-391-final-screens-count-level-ore/task-391-03-doc-level-ore.md) — doc: cancelled 2026-10-30, replaced by step 5
4. task-391-04-recap-ore-balance (vault/reference/tasks/task-391-final-screens-count-level-ore/task-391-04-recap-ore-balance.md) — cpp: stats screen Total Ore reads the balance held; cd19ba03 collected-total code removed when unused
5. task-391-05-doc-ore-balance (vault/reference/tasks/task-391-final-screens-count-level-ore/task-391-05-doc-ore-balance.md) — doc: run, transition, ore counter, shop and screen docs state the balance rule

## Relations
- [[tasks-done-index]] — parent index.
- [[task-391-01-level-ore-in-recap]] — step 1, cpp.
- [[task-391-02-run-won-ore-line]] — step 2, bp (cancelled).
- [[task-391-03-doc-level-ore]] — step 3, doc (cancelled).
- [[task-391-04-recap-ore-balance]] — step 4, cpp.
- [[task-391-05-doc-ore-balance]] — step 5, doc.
- [[intermission]] — the Total Ore definition the screens follow.
- [[task-383-level-end-total-ore]] — the task that made the single Total Ore line.
- [[task-390-run-won-back-to-main-menu]] — previous task (no shared widget since the re-plan).
- [[task-392-duskite-on-screen]] — next task, renames the ore labels.
