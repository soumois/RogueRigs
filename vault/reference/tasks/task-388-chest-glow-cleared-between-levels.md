---
id: task-388-chest-glow-cleared-between-levels
description: "Global task: Nothing of an opened bonus chest (interior glow, opening rays and sparks, lid slam ring) stays visible in the next level of the run."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Bonus chest glow removed between levels

- request: Owner, 2026-10-30, after testing the packaged build (Saved/Packaged/Windows/RogueRigs.exe), relayed verbatim by the main session (French), four points: "La lueur du coffre bonus reste entre les niveaux" / "Les particules de robot doivent aussi être retirées entre les niveaux (je pouvais voir des particules des robots tués au précédent niveau car j'ai passé rapidement à l'autre niveau)" / "Le dernier niveau doit ramener vers le main menu" / "Pour le dernier écran pour les ore sécurisés (ça n'est pas un autre mot que ore ?), il ne comptait pas ceux du level en cours." A screenshot of the arena was attached (aim chevrons visible, no issue there). This task is point 1 only: "La lueur du coffre bonus reste entre les niveaux". Reading (Planner): after a level where the bonus chest was opened, its glow is still seen in the next level. Levels 2-4 are prepared in the same world behind the cards screen (task-318 preload, no map load): the ClearingLevel step destroys the chest actor, but what the chest spawned into the world (NS_BonusChestOpen at the lid opening, NS_HeavySlamImpact at the slam, possibly the InnerLight or its material glow) is not owned by the chest; the world is paused during the intermission, so these effects resume when the next level starts. Hypothesis to confirm by a run, not a decided cause. Not in scope: robot particles (task-389), the chest opening look. Owner answers 2026-10-30 (relayed by the main session, translated): Q1 NO capture subtask on any task (388 to 393): the owner tests by himself; Q2, Q3, Q4 defaults accepted (389 after 388; "Main Menu" button on the run-won screen, loss screen keeps "Play again"; menu opens directly, same behaviour in all modes); the main-menu and run-structure GDD edits approved (applied 2026-10-30); Q5, Q6 defaults stay (both screens, same number as Total Ore); Q7 changed: every on-screen "Ore" becomes "Duskite", its own task (task-392); task-391 evidence added; new point: the miner looks over-lit (task-393).
- goal: When the next level of a run starts (preload path and map-travel fallback), no glow, light, ray, spark or ring from the previous level's bonus chest is present or playing; the chest opening in its own level is unchanged.
- acceptance: cpp: cause found by a -game run (chest opened with rr.Cheat.CompleteHack, then level change) and written in the change summary; at the ClearingLevel step every effect and light left by the bonus chest is removed (log line with the count), the fallback path shows none either; the opening still plays in its own level (log); build passes; docs updated by the doc subtask; lint clean; committed by path; owner checks the look himself.
- gdd_check: Read 2026-10-30: [[objective-bonus-chest]] (approved; opening: interior glow point light 3 s then fades, interior stays lit, rays and sparks, ground ring): nothing says the effects outlive the level; no conflict. [[intermission]] (provisional): next level prepared in the same world; no rule on leftover effects; no conflict. Tech read: [[sys-run-transition-preload]] (ClearingLevel destroys console, chest, shuttle and returns enemies, pickups, projectiles, ARRExplosionFX, turrets; world-spawned Niagara systems not listed), [[bp-bonus-chest]] (OnLidSlam and OnLidOpened use SpawnSystemAtLocation; InnerLight is a Mesh child). Open values: none. Scope M2. No capture subtask (owner Q1, 2026-10-30): the owner checks the result himself in the packaged build. Repo state 2026-10-30: RRPlayerController input fix committed by the owner (091bf23c); Config/DefaultGame.ini (DirectoriesToAlwaysCook) and Config/DefaultEditor.ini have uncommitted owner changes: leave them untouched and never commit them with this task (commit by path).
- confirmed_by_owner: yes (2026-10-30, owner answers relayed by the main session: Q1 no capture, Q2 to Q6 defaults, Q7 changed into task-392)
- milestone: M2
- next_step: task-388-01-clear-chest-effects
- started: 2026-10-30T20:43:17Z
- waiting_for: none
- result: open

## Questions
Decided 2026-10-30 (owner answers relayed by the main session); values provisional, owner 2026-10-30.
- Q1 Vérification par capture -game pour chaque tâche, autre choix : pas de capture, vérification par le propriétaire dans le build packagé. Answer: pas de capture, sur toutes les tâches (388 à 393) ; les sous-tâches doc restent.

## Subtasks
1. task-388-01-clear-chest-effects (vault/reference/tasks/task-388-chest-glow-cleared-between-levels/task-388-01-clear-chest-effects.md) — cpp: find what of the opened chest survives the level change and remove it at the ClearingLevel step
2. task-388-02-doc-chest-effects (vault/reference/tasks/task-388-chest-glow-cleared-between-levels/task-388-02-doc-chest-effects.md) — doc: update the preload and chest docs with the effects cleared between levels

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-388-01-clear-chest-effects]] — step 1, cpp.
- [[task-388-02-doc-chest-effects]] — step 2, doc.
- [[objective-bonus-chest]] — the chest opening whose effects must not outlive the level.
- [[sys-run-transition-preload]] — the ClearingLevel step changed here.
- [[task-389-robot-particles-cleared-between-levels]] — next task, same transition step, robot effects.
