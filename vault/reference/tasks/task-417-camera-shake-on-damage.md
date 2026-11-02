---
id: task-417-camera-shake-on-damage
description: "Global task: The camera shakes lightly each time the miner loses health, mixed with any explosion or slam shake playing."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Camera shake when the miner takes damage

- request: Owner, 2026-11-02, verbatim (French, relayed by the main session): "Ok, j'aimerais rajouter un shake caméra lorsqu'on prends des dégats et aussi lorsqu'une explosion se passe non loin de nous. (plus on est proche, plus le shake est fort, mais si la différence n'est pas énorme). Il faudrait définir une distance max où ça ne fait plus de shake également. Même chose, si on se trouve à X métre du gros robot lorsqu'il frappe au sol. Si pendant une explosion ou l'attaque au sol, on prends des dégats, ça doit se mixer correctement. Prépare les tâches et les questions." Split by the planner into four globals (one owner point each): task-416 mixing and distance falloff of the shared shake, task-417 shake on damage taken, task-418 shake on nearby explosions, task-419 shake on the heavy slam; run in that order (417 to 419 call the task-416 API).
- goal: Every hit that takes health from the miner plays a hit shake preset through the shared shake, mixed per task-416. Values per Q4. Depends on task-416 being done.
- acceptance: C++ builds; a hit preset class (5 uu, 0.2 s, 25 Hz, translation only, provisional owner 2026-11-02) plays from the miner on each OnDamageReceived with damage > 0, lethal hit included; hits refused by i-frames play nothing; it adds to an explosion or slam shake already playing (task-416 mixing); existing tests pass, no new test, no capture (task-416 Q3); doc nodes updated; lint clean; committed by path.
- gdd_check: Read 2026-11-02: [[sys-camera-shake]] (task-310): one shared C++ shake, URRCameraShakeLibrary::PlayCameraShake(WorldContext, Scale, ShakeClass) on the first local player camera manager, translation-only Perlin pattern, chest preset 0.2 s, (0,4,4) uu, 25 Hz, bSingleInstance true (a new call restarts it, no stacking), no distance falloff; its doc says "Heavy slam and explosions do not shake the camera (owner default Q21, 2026-10-23)": this request reverses that default on the owner's word, no conflict question needed. No gdd node describes camera shake today ([[visual-style]] camera line only); decided values go into [[visual-style]] (mixing), [[miner]] (hit), [[explosions]] (explosion shake) and [[enemy-heavy]] (slam shake) through the edits applied on confirmation. Values marked open before the questions: every strength, duration, falloff, max distance and the mixing rule. Milestone: juice is M3 work, done now on the owner request (task-310 was M2). Units: 1 m = 100 uu; a grid cell is 67 uu. [[miner]]: i-frames 0.25 s after a hit, so at most one hit shake per 0.25 s. [[sys-player]]: ARRMiner already binds the health set OnDamageReceived (i-frames, blink): the trigger point. [[sys-damage]]: OnDamageReceived fires only on consumed damage, never on a refused spec.
- confirmed_by_owner: no (plan and questions handed back 2026-11-02)
- milestone: M3
- next_step: task-417-01-hit-shake
- started: open
- waiting_for: owner confirmation of the plan
- result: open

## Questions
Asked 2026-11-02, each with a default; open until the owner answers.
- Q4 Hit shake: every hit that takes health from the miner (the lethal one too), 5 uu for 0.2 s (the chest is 4 uu), same strength whatever the damage (chaser 10 to barrel 40); a hit blocked by the i-frames does not shake. Default: this.

## Subtasks
1. task-417-01-hit-shake (vault/reference/tasks/task-417-camera-shake-on-damage/task-417-01-hit-shake.md) — cpp: hit shake preset played from the miner on each damage taken
2. task-417-02-doc (vault/reference/tasks/task-417-camera-shake-on-damage/task-417-02-doc.md) — doc: update sys-camera-shake and sys-player for the hit shake

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-417-01-hit-shake]] — step 1, cpp.
- [[task-417-02-doc]] — step 2, doc.
