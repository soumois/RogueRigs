---
id: task-389-robot-particles-cleared-between-levels
description: "Global task: Death puffs and debris of robots killed on the previous level no longer show when the next level starts."
parent: tasks-to-do-index
kind: task
status: todo
---
# Robot particles removed between levels

- request: Owner, 2026-10-30, after testing the packaged build (Saved/Packaged/Windows/RogueRigs.exe), relayed verbatim by the main session (French), four points: "La lueur du coffre bonus reste entre les niveaux" / "Les particules de robot doivent aussi être retirées entre les niveaux (je pouvais voir des particules des robots tués au précédent niveau car j'ai passé rapidement à l'autre niveau)" / "Le dernier niveau doit ramener vers le main menu" / "Pour le dernier écran pour les ore sécurisés (ça n'est pas un autre mot que ore ?), il ne comptait pas ceux du level en cours." A screenshot of the arena was attached (aim chevrons visible, no issue there). This task is point 2 only: "Les particules de robot doivent aussi être retirées entre les niveaux (je pouvais voir des particules des robots tués au précédent niveau car j'ai passé rapidement à l'autre niveau)". Reading (Planner): one-shot robot death effects (NS_RobotDeathPuff, NS_HeavyDeathPuff, NS_RobotDebris, NS_ShieldDebris) still playing at the extraction freeze with the paused world during the intermission and resume in the next level, prepared in the same world (task-318). Hypothesis to confirm by a run. Probably the same cause as task-388; separate task because the owner gave two points (Q2). Not in scope: other effects (explosions already cleared by ClearForNextLevel). Owner answers 2026-10-30 (relayed by the main session, translated): Q1 NO capture subtask on any task (388 to 393): the owner tests by himself; Q2, Q3, Q4 defaults accepted (389 after 388; "Main Menu" button on the run-won screen, loss screen keeps "Play again"; menu opens directly, same behaviour in all modes); the main-menu and run-structure GDD edits approved (applied 2026-10-30); Q5, Q6 defaults stay (both screens, same number as Total Ore); Q7 changed: every on-screen "Ore" becomes "Duskite", its own task (task-392); task-391 evidence added; new point: the miner looks over-lit (task-393).
- goal: When the next level of a run starts, no death puff, debris or shield debris of robots of the previous level is present or playing; death effects during a level are unchanged.
- acceptance: cpp: cause confirmed by a -game run (robots killed just before the extraction, quick level change) and written in the change summary; at the ClearingLevel step every robot death effect still alive is removed (log line with the count), or, if task-388 already removes them, a log proving it and no extra code; death effects still spawn during a level; build passes; docs updated; lint clean; committed by path; owner checks the look himself.
- gdd_check: Read 2026-10-30: [[intermission]] (next level prepared in the same world; no rule on leftover effects): no conflict. Tech read: [[sys-run-transition-preload]] (ClearingLevel returns enemies to the pool and clears explosion FX; robot death Niagara one-shots not listed). Effect nodes: [[bp-robot-death-puff]], [[bp-heavy-death-puff]], [[bp-robot-debris]], [[bp-shield-debris]]. Open values: none. Scope M2. Runs after task-388 (Q2). No capture subtask (owner Q1, 2026-10-30): the owner checks the result himself in the packaged build. Repo state 2026-10-30: RRPlayerController input fix committed by the owner (091bf23c); Config/DefaultGame.ini (DirectoriesToAlwaysCook) and Config/DefaultEditor.ini have uncommitted owner changes: leave them untouched and never commit them with this task (commit by path).
- confirmed_by_owner: yes (2026-10-30, owner answers relayed by the main session: Q1 no capture, Q2 to Q6 defaults, Q7 changed into task-392)
- milestone: M2
- next_step: task-389-01-clear-robot-effects
- started: open
- waiting_for: none
- result: open

## Questions
Decided 2026-10-30 (owner answers relayed by the main session); values provisional, owner 2026-10-30.
- Q2 Deux tâches séparées ; la 389 passe après la 388 et, si le correctif de la 388 retire déjà les particules des robots, elle ne fait que le prouver par un log, sans code de plus. Answer: oui (défaut).

## Subtasks
1. task-389-01-clear-robot-effects (vault/reference/tasks/task-389-robot-particles-cleared-between-levels/task-389-01-clear-robot-effects.md) — cpp: remove the robot death puffs and debris still playing when the next level is prepared
2. task-389-02-doc-robot-effects (vault/reference/tasks/task-389-robot-particles-cleared-between-levels/task-389-02-doc-robot-effects.md) — doc: update the preload and enemies docs with the robot effects cleared between levels

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-389-01-clear-robot-effects]] — step 1, cpp.
- [[task-389-02-doc-robot-effects]] — step 2, doc.
- [[task-388-chest-glow-cleared-between-levels]] — previous task, same transition step, chest effects.
- [[sys-run-transition-preload]] — the ClearingLevel step changed here.
