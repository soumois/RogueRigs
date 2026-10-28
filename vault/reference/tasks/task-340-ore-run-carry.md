---
id: task-340-ore-run-carry
description: "Global task: Duskite ore is never reset between levels: the miner sees and spends his whole run total, and every run starts at 0."
parent: tasks-to-do-index
kind: task
status: todo
---
# Ore kept for the whole run, reset at run end

- request: Point 1 of the owner message: "Crée une tâche pour modifier le GDD et le code, les cristaux mauve ne sont plus reset entre les niveaux. Cette ressource ne sert que pour la run en cours. (et c'est frustrant de ne pas pouvoir cumuler les cristaux sur deux niveaux pour faire de gros achat)" (owner, 2026-10-28; full message: "* Crée une tâche pour modifier le GDD et le code, les cristaux mauve ne sont plus reset entre les niveaux. Cette ressource ne sert que pour la run en cours. (et c'est frustrant de ne pas pouvoir cumuler les cristaux sur deux niveaux pour faire de gros achat) / Ok voilà l'écran de sélection des cartes entre deux niveaux, il va falloir faire son design et son gamefeeling. Pour cela: * Il me faut le prompt pour chatgpt qui me fera des propositions et je discuterais avec lui * Lorsque j'achète une carte, je veux mes cristaux qui partent du compte et vont vers le bouton * Il faut un côté juicy aussi lorsqu'on clique sur le bouton (autre que l'effet d'avant) * Il faut le même effet que lorsque je clique sur la carte (sauf l'effet qui bouge jusqu'à l'arme ou l'xp"). Owner answer 2026-10-28 (relayed by the main session): "OK, tout par défaut" = every default of Q1 to Q11 accepted (listed in gdd_check).
- goal: Duskite ore is never reset between levels: the miner sees and spends his whole run total, and every run starts at 0.
- acceptance: ore collected on level 1 and secured by extraction is still shown and spendable on level 2, at the level-2 level-up reroll and at the next shop; the in-level HUD ore counter shows the run total (Q1); a new run (loss, win, restart, main menu New Run) starts at 0 ore; the existing tests that use ore still pass; -game capture of the level 2 HUD and the next shop sent to the owner; ore-economy and run-structure updated; docs updated; lint clean; committed.
- gdd_check: read 2026-10-28: [[ore-economy]] (approved), [[run-structure]] (approved), [[run-shop]], [[intermission]], [[extraction]]; tech [[sys-objectives-ore-counter]], [[sys-run]], [[sys-run-shop]], [[wbp-ore-counter]]; code read (no change): `Source/RogueRigs/Objectives/RROreBankSubsystem.h`. Findings: run-structure already says ore carries into the next level; the code keeps secured ore in `URROreBankSubsystem` across levels, but (a) the HUD counter WBP_OreCounter shows only ore carried this level, which starts at 0 on every level (likely what the owner saw as a reset), (b) the bank is never cleared, so ore also survives into the next run of the same session, contrary to "ne sert que pour la run en cours". Open value touched: ore-economy "losing a share of secured ore when a run fails: open". Missing the shuttle loses the run, so carried and secured ore no longer differ for the player. Questions (defaults): Q1 the in-level HUD counter shows carried + secured ore (the run total, what can be spent); Q2 all ore is reset to 0 when a run ends (lost, won, restart, New Run), which closes the open "share lost on a failed run" line (all of it is lost); ore-economy and run-structure rewritten accordingly by the Planner once answered; Q3 if the cpp agent finds a real reset of secured ore between levels, it fixes it in the same subtask. Owner answer 2026-10-28: "OK, tout par défaut": every default above is DECIDED (values provisional, owner 2026-10-28); GDD updated by the Planner the same day.
- confirmed_by_owner: yes (2026-10-28, "OK, tout par défaut" to Q1-Q11 and the plan, relayed by the main session)
- milestone: M2
- next_step: task-340-01-ore-run-reset
- started: open
- waiting_for: none
- result: open

## Subtasks
1. task-340-01-ore-run-reset (vault/reference/tasks/task-340-ore-run-carry/task-340-01-ore-run-reset.md) — cpp: reproduce the reset seen by the owner, keep ore across levels, clear it at each new run.
2. task-340-02-ore-counter-total (vault/reference/tasks/task-340-ore-run-carry/task-340-02-ore-counter-total.md) — bp: WBP_OreCounter shows carried + secured ore.
3. task-340-03-ore-carry-capture (vault/reference/tasks/task-340-ore-run-carry/task-340-03-ore-carry-capture.md) — cpp: -game capture: level 2 HUD and next shop show the carried total, new run at 0.
4. task-340-04-ore-carry-doc (vault/reference/tasks/task-340-ore-run-carry/task-340-04-ore-carry-doc.md) — doc: update sys-objectives-ore-counter, sys-run, wbp-ore-counter.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-340-01-ore-run-reset]] — step 1, cpp.
- [[task-340-02-ore-counter-total]] — step 2, bp.
- [[task-340-03-ore-carry-capture]] — step 3, cpp.
- [[task-340-04-ore-carry-doc]] — step 4, doc.
