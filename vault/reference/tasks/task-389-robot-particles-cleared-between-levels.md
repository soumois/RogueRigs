---
id: task-389-robot-particles-cleared-between-levels
description: "Global task: Death puffs and debris of robots killed on the previous level no longer show when the next level starts."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Robot particles removed between levels

- request: Owner, 2026-10-30, after testing the packaged build (Saved/Packaged/Windows/RogueRigs.exe), relayed verbatim by the main session (French), four points: "La lueur du coffre bonus reste entre les niveaux" / "Les particules de robot doivent aussi être retirées entre les niveaux (je pouvais voir des particules des robots tués au précédent niveau car j'ai passé rapidement à l'autre niveau)" / "Le dernier niveau doit ramener vers le main menu" / "Pour le dernier écran pour les ore sécurisés (ça n'est pas un autre mot que ore ?), il ne comptait pas ceux du level en cours." A screenshot of the arena was attached (aim chevrons visible, no issue there). This task is point 2 only: "Les particules de robot doivent aussi être retirées entre les niveaux (je pouvais voir des particules des robots tués au précédent niveau car j'ai passé rapidement à l'autre niveau)". Reading (Planner): one-shot effects of robot deaths (NS_RobotDeathPuff, NS_HeavyDeathPuff, NS_RobotDebris, NS_ShieldDebris) still playing at the extraction are frozen with the paused world during the intermission and resume in the next level, prepared in the same world (task-318). Hypothesis, to confirm by a run. Probably the same cause as task-388; kept a separate task as the owner gave two points (Q2). Not in scope: other effects (explosions are already cleared by ClearForNextLevel).
- goal: When the next level of a run starts, no death puff, debris or shield debris of robots of the previous level is present or playing; robot death effects in the current level are unchanged.
- acceptance: cpp: cause confirmed by a -game run (robots killed just before the extraction, quick level change) and written in the change summary; at the ClearingLevel step every robot death effect (puff, heavy puff, debris, shield debris) still alive is removed (log line with the count), or, if task-388 already removes them, a log proving it and no extra code; death effects still play during a level; build passes; capture checklist C1 to C2 seen; docs updated by the doc subtask; lint clean; committed by path.
- gdd_check: Read 2026-10-30: [[intermission]] (next level prepared in the same world; no rule on leftover effects): no conflict. Tech read: [[sys-run-transition-preload]] (ClearingLevel returns enemies to the pool and clears explosion FX; Niagara one-shots spawned at robot death are not listed). Effect nodes: [[bp-robot-death-puff]], [[bp-heavy-death-puff]], [[bp-robot-debris]], [[bp-shield-debris]]. Open values: none. Scope M2. Runs after task-388 (same function likely touched; Q2). Capture per Q1 of task-388. Repo state 2026-10-30: uncommitted changes not made by this task in Source/RogueRigs/Player/RRPlayerController.cpp/.h (RestoreGameInputMode, packaged input fix), Config/DefaultGame.ini and Config/DefaultEditor.ini: leave them untouched and never commit them with this task (commit by path). task-387 (art, wait-answers) shares no file.
- confirmed_by_owner: no (plan and questions handed back 2026-10-30)
- milestone: M2
- next_step: task-389-01-clear-robot-effects
- started: open
- waiting_for: owner confirmation of the plan
- result: open

## Questions
Asked 2026-10-30, each with a default; open until the owner answers.
- Q2 Les points 1 et 2 ont sans doute la même cause (des effets lancés dans le monde, figés pendant l'écran de fin puis repris au niveau suivant, préparé dans le même monde). Deux tâches séparées comme vos deux points ; la 389 passe après la 388 et, si le correctif de la 388 retire déjà les particules des robots, elle ne fait que le prouver par un log et une capture, sans code de plus. Default: oui.

## Subtasks
1. task-389-01-clear-robot-effects (vault/reference/tasks/task-389-robot-particles-cleared-between-levels/task-389-01-clear-robot-effects.md) — cpp: remove the robot death puffs and debris still playing when the next level is prepared
2. task-389-02-robot-particles-capture (vault/reference/tasks/task-389-robot-particles-cleared-between-levels/task-389-02-robot-particles-capture.md) — capture: -game frames: robots killed just before extraction, no particles on level 2
3. task-389-03-doc-robot-effects (vault/reference/tasks/task-389-robot-particles-cleared-between-levels/task-389-03-doc-robot-effects.md) — doc: update the preload and enemies docs with the robot effects cleared between levels

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-389-01-clear-robot-effects]] — step 1, cpp.
- [[task-389-02-robot-particles-capture]] — step 2, capture.
- [[task-389-03-doc-robot-effects]] — step 3, doc.
- [[task-388-chest-glow-cleared-between-levels]] — previous task, same transition step, chest effects.
- [[sys-run-transition-preload]] — the ClearingLevel step changed here.
