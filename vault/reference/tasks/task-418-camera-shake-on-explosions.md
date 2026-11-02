---
id: task-418-camera-shake-on-explosions
description: "Global task: Each barrel or kamikaze explosion shakes the camera, a bit stronger when closer, nothing beyond a max distance."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Camera shake from nearby explosions

- request: Owner, 2026-11-02, verbatim (French, relayed by the main session): "Ok, j'aimerais rajouter un shake caméra lorsqu'on prends des dégats et aussi lorsqu'une explosion se passe non loin de nous. (plus on est proche, plus le shake est fort, mais si la différence n'est pas énorme). Il faudrait définir une distance max où ça ne fait plus de shake également. Même chose, si on se trouve à X métre du gros robot lorsqu'il frappe au sol. Si pendant une explosion ou l'attaque au sol, on prends des dégats, ça doit se mixer correctement. Prépare les tâches et les questions." Split by the planner into four globals (one owner point each): task-416 mixing and distance falloff of the shared shake, task-417 shake on damage taken, task-418 shake on nearby explosions, task-419 shake on the heavy slam; run in that order (417 to 419 call the task-416 API).
- goal: Each resolved explosion plays an explosion shake at its centre through the task-416 located call: strength per source, falloff to 50 % at the max distance, nothing beyond. Values per Q5 to Q7. Depends on task-416 being done.
- acceptance: C++ builds; when URRExplosionSubsystem resolves an explosion it plays the explosion preset (0.35 s, 25 Hz, translation only) at the centre with strength barrel 10 uu, kamikaze 7 uu and max distance 2 x the row radius (barrel 1400 uu, kamikaze 624 uu), scale 1 at the centre, 0.5 at the max distance, nothing beyond; no shake during barrel or kamikaze fuses; a chain shakes once per link; mixes with the hit shake (task-416); values provisional owner 2026-11-02 and tunable per source; existing explosion tests pass, no new test, no capture (task-416 Q3); doc nodes updated; lint clean; committed by path.
- gdd_check: Read 2026-11-02: [[sys-camera-shake]] (task-310): one shared C++ shake, URRCameraShakeLibrary::PlayCameraShake(WorldContext, Scale, ShakeClass) on the first local player camera manager, translation-only Perlin pattern, chest preset 0.2 s, (0,4,4) uu, 25 Hz, bSingleInstance true (a new call restarts it, no stacking), no distance falloff; its doc says "Heavy slam and explosions do not shake the camera (owner default Q21, 2026-10-23)": this request reverses that default on the owner's word, no conflict question needed. No gdd node describes camera shake today ([[visual-style]] camera line only); decided values go into [[visual-style]] (mixing), [[miner]] (hit), [[explosions]] (explosion shake) and [[enemy-heavy]] (slam shake) through the edits applied on confirmation. Values marked open before the questions: every strength, duration, falloff, max distance and the mixing rule. Milestone: juice is M3 work, done now on the owner request (task-310 was M2). Units: 1 m = 100 uu; a grid cell is 67 uu. [[explosions]]: sources barrel and kamikaze, one link per tick with a 0.08 s chain delay, barrel fuse 1.5 s. [[barrel]] radius 700 uu, [[enemy-kamikaze]] radius 312 uu. [[sys-explosions]]: single resolution path (characters, FX, then OnExplosion), per-row ExplosionStats.csv: the trigger point and where per-source values can live.
- confirmed_by_owner: no (plan and questions handed back 2026-11-02)
- milestone: M3
- next_step: task-418-01-explosion-shake
- started: open
- waiting_for: owner confirmation of the plan
- result: open

## Questions
Asked 2026-11-02, each with a default; open until the owner answers.
- Q5 Distance falloff ("plus on est proche, plus fort, mais la différence n'est pas énorme"), for explosions and the slam: full strength at the centre, falling in a straight line to 50 % at the max distance, nothing beyond. Default: 100 % to 50 %.
- Q6 Explosion shake: on the explosion itself, not during the barrel or kamikaze fuse; at the centre barrel 10 uu, kamikaze 7 uu (the barrel blast is twice as wide), 0.35 s; in a chain each explosion shakes again. Default: this.
- Q7 Explosion max distance: 2 x the explosion radius: barrel 1400 uu (14 m), kamikaze 624 uu (about 6 m). Default: 2 x radius.

## Subtasks
1. task-418-01-explosion-shake (vault/reference/tasks/task-418-camera-shake-on-explosions/task-418-01-explosion-shake.md) — cpp: explosion shake preset played at each resolved explosion with distance falloff
2. task-418-02-doc (vault/reference/tasks/task-418-camera-shake-on-explosions/task-418-02-doc.md) — doc: update sys-explosions and sys-camera-shake for the explosion shake

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-418-01-explosion-shake]] — step 1, cpp.
- [[task-418-02-doc]] — step 2, doc.
