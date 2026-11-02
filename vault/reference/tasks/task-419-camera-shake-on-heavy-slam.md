---
id: task-419-camera-shake-on-heavy-slam
description: "Global task: The heavy ground slam shakes the camera when the miner is within a max distance, a bit stronger when closer."
parent: tasks-done-index
kind: task
status: done
---
# Camera shake from the heavy ground slam

- request: Owner, 2026-11-02, verbatim (French, relayed by the main session): "Ok, j'aimerais rajouter un shake caméra lorsqu'on prends des dégats et aussi lorsqu'une explosion se passe non loin de nous. (plus on est proche, plus le shake est fort, mais si la différence n'est pas énorme). Il faudrait définir une distance max où ça ne fait plus de shake également. Même chose, si on se trouve à X métre du gros robot lorsqu'il frappe au sol. Si pendant une explosion ou l'attaque au sol, on prends des dégats, ça doit se mixer correctement. Prépare les tâches et les questions." Split by the planner into four globals (one owner point each): task-416 mixing and distance falloff of the shared shake, task-417 shake on damage taken, task-418 shake on nearby explosions, task-419 shake on the heavy slam; run in that order (417 to 419 call the task-416 API). Owner answer 2026-11-02 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q8, Q9 accepted.
- goal: The heavy slam frame plays a slam shake at the heavy feet through the task-416 located call, falloff to 50 % at the max distance, nothing beyond, hit or not. Values per Q5 (task-418), Q8 and Q9. Depends on task-416 being done.
- acceptance: C++ builds; ARRHeavy plays the slam preset (8 uu at the heavy, 0.3 s, 25 Hz, translation only) on the slam frame at its feet, max distance 2 x slam radius (722 uu), scale 1 at the heavy, 0.5 at the max distance, nothing beyond; nothing during the telegraph; a slam that also hits the miner plays the slam and hit shakes mixed (task-416); values provisional owner 2026-11-02; existing heavy tests pass, no new test, no capture (task-416 Q3); doc nodes updated; lint clean; committed by path.
- gdd_check: Read 2026-11-02: [[sys-camera-shake]] (task-310): one shared C++ shake, URRCameraShakeLibrary::PlayCameraShake(WorldContext, Scale, ShakeClass) on the first local player camera manager, translation-only Perlin pattern, chest preset 0.2 s, (0,4,4) uu, 25 Hz, bSingleInstance true (a new call restarts it, no stacking), no distance falloff; its doc says "Heavy slam and explosions do not shake the camera (owner default Q21, 2026-10-23)": this request reverses that default on the owner's word, no conflict question needed. No gdd node describes camera shake today ([[visual-style]] camera line only); decided values go into [[visual-style]] (mixing), [[miner]] (hit), [[explosions]] (explosion shake) and [[enemy-heavy]] (slam shake) through the edits applied on confirmation. Values marked open before the questions: every strength, duration, falloff, max distance and the mixing rule. Milestone: juice is M3 work, done now on the owner request (task-310 was M2). Units: 1 m = 100 uu; a grid cell is 67 uu. [[enemy-heavy]]: slam radius 361 uu, telegraph 1.0 s, one slam every 2 s, at most 3 alive, slam damage 25 to the miner within the radius. [[sys-enemies-heavy]]: PerformSlam (slam frame, OnSlam(Location = feet, Radius)) is the trigger point. Owner "X mètre" is the slam max distance: Q9.
- confirmed_by_owner: yes (2026-11-02, "ok")
- milestone: M3
- next_step: none
- started: 2026-11-02T18:47:37Z
- waiting_for: none
- result: Done. Done 2a540c48: sys-enemies-heavy and sys-camera-shake document the slam shake (8 uu, 0.3 s, 25 Hz, 722 uu max, falloff 0.5 at max).

## Questions
Decided 2026-11-02 (owner "ok"): every default below is accepted; values provisional, owner 2026-11-02.
- Q8 Slam shake: on the slam frame (fists hit the ground), whether or not the miner is hit, nothing during the 1 s telegraph; 8 uu at the heavy, 0.3 s; falloff as Q5. Default: this.
- Q9 Slam max distance ("X mètre"): 2 x the slam radius = 722 uu (about 7 m; the damage radius is 361 uu). Default: 722 uu.

## Subtasks
1. task-419-01-slam-shake (vault/reference/tasks/task-419-camera-shake-on-heavy-slam/task-419-01-slam-shake.md) — cpp: slam shake preset played at the heavy on the slam frame with distance falloff
2. task-419-02-doc (vault/reference/tasks/task-419-camera-shake-on-heavy-slam/task-419-02-doc.md) — doc: update sys-enemies-heavy and sys-camera-shake for the slam shake

## Relations
- [[tasks-done-index]] — parent index.
- [[task-419-01-slam-shake]] — step 1, cpp.
- [[task-419-02-doc]] — step 2, doc.
