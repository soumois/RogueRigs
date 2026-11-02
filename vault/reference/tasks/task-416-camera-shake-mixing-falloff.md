---
id: task-416-camera-shake-mixing-falloff
description: "Global task: shared camera shake: different sources add up with a cap; a shake at a world point fades with distance up to a max."
parent: tasks-to-do-index
kind: task
status: todo
---
# Camera shakes mix and fade with distance

- request: Owner, 2026-11-02, verbatim (French, relayed by the main session): "Ok, j'aimerais rajouter un shake caméra lorsqu'on prends des dégats et aussi lorsqu'une explosion se passe non loin de nous. (plus on est proche, plus le shake est fort, mais si la différence n'est pas énorme). Il faudrait définir une distance max où ça ne fait plus de shake également. Même chose, si on se trouve à X métre du gros robot lorsqu'il frappe au sol. Si pendant une explosion ou l'attaque au sol, on prends des dégats, ça doit se mixer correctement. Prépare les tâches et les questions." Split by the planner into four globals (one owner point each): task-416 mixing and distance falloff of the shared shake, task-417 shake on damage taken, task-418 shake on nearby explosions, task-419 shake on the heavy slam; run in that order (417 to 419 call the task-416 API). Owner answer 2026-11-02 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q1, Q2, Q3 accepted.
- goal: The shared camera shake ([[sys-camera-shake]]) plays several shakes together (different sources add up, same source restarts at the stronger strength, total capped) and offers a call that plays a shake at a world point, scaled by the distance to the miner camera target with a max distance. Values per the Questions.
- acceptance: C++ builds; URRCameraShakeLibrary offers a located call (epicenter, max distance, strength) whose scale is 1 at the epicenter, falls linearly to 0.5 at the max distance and is 0 (nothing played) beyond, distance measured in 2D from the miner; shakes of different preset classes play together and add up; a new shake of the same class restarts it at the stronger of the old and new scales; the summed translation never exceeds 12 uu; the chest shake (task-309) still plays as before; no rotation; existing tests pass, no new test, no capture (Q3); doc node updated; lint clean; committed by path.
- gdd_check: Read 2026-11-02: [[sys-camera-shake]] (task-310): one shared C++ shake, URRCameraShakeLibrary::PlayCameraShake(WorldContext, Scale, ShakeClass) on the first local player camera manager, translation-only Perlin pattern, chest preset 0.2 s, (0,4,4) uu, 25 Hz, bSingleInstance true (a new call restarts it, no stacking), no distance falloff; its doc says "Heavy slam and explosions do not shake the camera (owner default Q21, 2026-10-23)": this request reverses that default on the owner's word, no conflict question needed. No gdd node describes camera shake today ([[visual-style]] camera line only); decided values go into [[visual-style]] (mixing), [[miner]] (hit), [[explosions]] (explosion shake) and [[enemy-heavy]] (slam shake) through the edits applied on confirmation. Values marked open before the questions: every strength, duration, falloff, max distance and the mixing rule. Milestone: juice is M3 work, done now on the owner request (task-310 was M2). Units: 1 m = 100 uu; a grid cell is 67 uu. This global holds the mixing and the distance falloff that tasks 417 to 419 use.
- confirmed_by_owner: yes (2026-11-02, "ok")
- milestone: M3
- next_step: task-416-01-shake-mixing-falloff
- started: open
- waiting_for: none
- result: open

## Questions
Decided 2026-11-02 (owner "ok"): every default below is accepted; values provisional, owner 2026-11-02.
- Q1 Plan: four globals run one after the other: 416 mixing and distance falloff (shared shake), 417 shake on damage taken, 418 shake on nearby explosions, 419 shake on the heavy slam; each has one cpp subtask and one doc subtask. Default: this plan.
- Q2 Mixing ("ça doit se mixer correctement"): shakes of different sources (hit, explosion, slam, chest) add up; a second shake of the same source while one plays restarts it at the stronger of the two; the sum is capped at 12 uu so a pile-up (barrel chain + hit + slam) stays readable; translation only, no rotation, like the chest. Default: this rule.
- Q3 Checking: a shake is camera motion and does not read on still frames, so no capture subtask: build green, existing tests pass, no new test; you feel it in play and every value stays provisional for tuning. Default: no capture.

## Subtasks
1. task-416-01-shake-mixing-falloff (vault/reference/tasks/task-416-camera-shake-mixing-falloff/task-416-01-shake-mixing-falloff.md) — cpp: additive mixing with cap and located shake with distance falloff in the shared camera shake
2. task-416-02-doc (vault/reference/tasks/task-416-camera-shake-mixing-falloff/task-416-02-doc.md) — doc: update sys-camera-shake for mixing and the located call

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-416-01-shake-mixing-falloff]] — step 1, cpp.
- [[task-416-02-doc]] — step 2, doc.
