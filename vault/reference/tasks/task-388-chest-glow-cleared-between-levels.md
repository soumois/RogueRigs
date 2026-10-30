---
id: task-388-chest-glow-cleared-between-levels
description: "Global task: Nothing of an opened bonus chest (interior glow, opening rays and sparks, lid slam ring) stays visible in the next level of the run."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Bonus chest glow removed between levels

- request: Owner, 2026-10-30, after testing the packaged build (Saved/Packaged/Windows/RogueRigs.exe), relayed verbatim by the main session (French), four points: "La lueur du coffre bonus reste entre les niveaux" / "Les particules de robot doivent aussi être retirées entre les niveaux (je pouvais voir des particules des robots tués au précédent niveau car j'ai passé rapidement à l'autre niveau)" / "Le dernier niveau doit ramener vers le main menu" / "Pour le dernier écran pour les ore sécurisés (ça n'est pas un autre mot que ore ?), il ne comptait pas ceux du level en cours." A screenshot of the arena was attached (aim chevrons visible, no issue there). This task is point 1 only: "La lueur du coffre bonus reste entre les niveaux". Reading (Planner): after a level where the bonus chest was opened, its glow is still seen in the next level. Levels 2-4 are normally prepared in the same world behind the cards screen (task-318 preload, no map load): the ClearingLevel step destroys the chest actor, but what the chest spawned into the world (NS_BonusChestOpen at the lid opening, NS_HeavySlamImpact at the slam, possibly the InnerLight or its material glow) is not owned by the chest; the world is paused during the intermission, so these effects resume when the next level starts. This is a hypothesis to confirm by a run, not a decided cause. Not in scope: robot particles (task-389), the chest opening look itself.
- goal: When the next level of a run starts (preload path and map-travel fallback), no glow, light, ray, spark or ring from the previous level's bonus chest is present or playing; the chest opening in the current level is unchanged.
- acceptance: cpp: the cause is found by a -game run (chest opened with rr.Cheat.CompleteHack, then level change) and written in the change summary; at the ClearingLevel step of the preload every effect and light left by the bonus chest is removed (log line with the count removed), and the map-travel fallback shows none either; the chest opening (glow, rays, sparks, slam ring) still plays in its own level; build passes; capture checklist C1 to C3 seen; sys docs updated by the doc subtask; lint clean; committed by path.
- gdd_check: Read 2026-10-30: [[objective-bonus-chest]] (approved; opening: interior glow point light 3 s then fades, interior stays lit, rays and sparks, ground ring): nothing says the effects outlive the level; no conflict. [[intermission]] (provisional): the next level is prepared in the same world behind the cards screen; no rule on leftover effects; no conflict. Tech read: [[sys-run-transition-preload]] (ClearingLevel destroys console, chest, shuttle and returns enemies, pickups, projectiles, ARRExplosionFX, turrets; world-spawned Niagara systems are not listed), [[bp-bonus-chest]] (OnLidSlam and OnLidOpened use SpawnSystemAtLocation; InnerLight is a Mesh child). Open values: none. Scope M2. Verification by capture is asked in Q1 (applies to tasks 388 to 391). Repo state 2026-10-30: uncommitted changes not made by this task in Source/RogueRigs/Player/RRPlayerController.cpp/.h (RestoreGameInputMode, packaged input fix), Config/DefaultGame.ini and Config/DefaultEditor.ini: leave them untouched and never commit them with this task (commit by path). task-387 (art, wait-answers) shares no file.
- confirmed_by_owner: no (plan and questions handed back 2026-10-30)
- milestone: M2
- next_step: task-388-01-clear-chest-effects
- started: open
- waiting_for: owner confirmation of the plan
- result: open

## Questions
Asked 2026-10-30, each with a default; open until the owner answers.
- Q1 Vérification (pour les quatre tâches 388 à 391) : chaque tâche finit par une capture -game faite par l'agent capture avec une liste de points C1..Cn à voir sur les images (règle du 2026-10-16 : un changement visible se vérifie à l'écran), puis la mise à jour des docs. Autre choix : pas de capture, vous vérifiez vous-même dans le build packagé. Default: capture pour chaque tâche.

## Subtasks
1. task-388-01-clear-chest-effects (vault/reference/tasks/task-388-chest-glow-cleared-between-levels/task-388-01-clear-chest-effects.md) — cpp: find what of the opened chest survives the level change and remove it at the ClearingLevel step
2. task-388-02-chest-glow-capture (vault/reference/tasks/task-388-chest-glow-cleared-between-levels/task-388-02-chest-glow-capture.md) — capture: -game frames: chest opened on level 1, nothing of it on level 2
3. task-388-03-doc-chest-effects (vault/reference/tasks/task-388-chest-glow-cleared-between-levels/task-388-03-doc-chest-effects.md) — doc: update the preload and chest docs with the effects cleared between levels

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-388-01-clear-chest-effects]] — step 1, cpp.
- [[task-388-02-chest-glow-capture]] — step 2, capture.
- [[task-388-03-doc-chest-effects]] — step 3, doc.
- [[objective-bonus-chest]] — the chest opening whose effects must not outlive the level.
- [[sys-run-transition-preload]] — the ClearingLevel step changed here.
- [[task-389-robot-particles-cleared-between-levels]] — next task, same transition step, robot effects.
