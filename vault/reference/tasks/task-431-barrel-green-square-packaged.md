---
id: task-431-barrel-green-square-packaged
description: "Global task: no green default-decal square at any barrel fuse or explosion in a packaged build, second and later runs of one process included."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Barrel explosion green square in the packaged build

- request: Owner 2026-11-04, verbatim: "J'ai eu le bug du carré vert lors de l'explosion du baril, mais je ne l'ai pas eu la première fois et j'étais en build." (I got the green square bug at the barrel explosion, but not the first time, and I was in a build.) Context: [[task-201-decal-first-frame-green]] removed the one-frame green default-decal box in `-game` runs by warming the decal materials at level start (ARRDecalWarmUp, 2 s); its result says "cooked build and cold cache unverified". The owner now sees the square in a build, not on the first explosion: the warm-up does not cover this case. Bug fix only: no new test, no new effect, game rules unchanged. Owner answers 2026-11-04, verbatim: "J'ai relancé le build, je n'avais plus le souci. Le souci vient quand je meurs une fois. Une fois le carré vert là, il revient tout le temps. Pas dans l'éditeur, un vrai build (et pas en développement)." (I relaunched the build, no problem any more. The problem comes when I die once. Once the green square is there, it comes back all the time. Not in the editor, a real build, not Development.) Read: a packaged non-Development (Shipping) build; clean on a fresh launch; after one miner death (and the restart that follows) the green square shows on every barrel explosion. Q1 to Q4 settled by this answer (Q4: no video given, the agent reproduces). Owner extra detail 2026-11-04 (relayed paraphrase): quitting a run through the pause menu and starting a new game without closing the application also keeps the green square; only a full relaunch of the build clears it. Read: the defect starts with the second run (any run end, death or pause-menu exit) in the same process and survives every later level or run load; something kept across worlds (static, subsystem, cached material, PSO or pooled object) is not valid for the new world. This answers Q5. Owner answer 2026-11-04 (relayed by the main session, verbatim): "Oui mais prépare la tâche seulement." = the plan and the defaults of Q6 accepted.
- goal: No green default-decal square at the barrel fuse start (danger zone) or at the explosion (shockwave ring) in a packaged build, in the first run and in every later run of the same process (after a death and restart, or a pause-menu exit and New Run).
- acceptance: The defect is first reproduced in a staged package in the second run of one process (frames and log lines) before any change, or reported not reproduced after 2 runs per configuration (Development, then Test; then stop, owner wait); the cause is named with evidence; after the fix, in the same package configuration, frames at fuse start and explosion of barrel explosions in the first run, after a death and restart, after a pause-menu exit and New Run, and in a third run show no green square; a Shipping package (BuildCookRun of [[build-toolchain]], `-clientconfig=Shipping`) builds with exit 0 and is staged for the owner's check (Q6); the task-201 `-game` check stays clean; existing explosion, barrel and telegraph tests pass (only broken expectations updated, no new test); game rules unchanged; committed by path.
- gdd_check: [[barrel]] and [[explosions]] read 2026-11-04: a render defect, no rule change, no `open` value needed, no conflict. Earlier work: [[task-200-decal-green-square-check]] (diagnosis in -game) and [[task-201-decal-first-frame-green]] (warm-up fix, uncooked cause: incomplete shader map, fallback to the engine default deferred decal; cooked build unverified). The packaged-build frames are taken by the cpp subtask itself, as in task-201: capture.js and [[agent-capture]] only launch `-game` from the editor, and the defect is seen in a build.
- confirmed_by_owner: yes (2026-11-04, "Oui mais prépare la tâche seulement.")
- milestone: M2
- next_step: task-431-02-packaged-green-square-doc
- started: 2026-11-05T14:41:52Z
- waiting_for: none
- result: open

## Questions
Decided 2026-11-04 (owner "Oui mais prépare la tâche seulement."): the defaults of Q6 are accepted; values provisional, owner 2026-11-04.
- Q1 (answered 2026-11-04, see request) Build kind: a real packaged build, not Development, not the editor.
- Q2 (answered 2026-11-04) When: only after the miner died once; from then on on every barrel explosion; a fresh launch is clean.
- Q3 (answered 2026-11-04) Look: it comes back each time once present; one flash per explosion assumed, the agent describes it on its frames.
- Q4 (answered 2026-11-04) Video: none given; the agent reproduces it.
- Q5 (answered 2026-11-04, see request) Run end path: both a death then restart and a pause-menu exit then New Run keep the square; only a full relaunch clears it.
- Q6 Final check: debug commands do not exist in a Shipping build, so the agent reproduces and checks in a Development or Test package (Test is close to Shipping), builds a Shipping package after the fix and leaves it for you to play (die once, then blow barrels). Default: yes.

## Subtasks
1. task-431-01-packaged-green-square-fix (vault/reference/tasks/task-431-barrel-green-square-packaged/task-431-01-packaged-green-square-fix.md) — cpp: reproduce the green square from the second run of one packaged process, find the cause, fix it, verify in packaged frames
2. task-431-02-packaged-green-square-doc (vault/reference/tasks/task-431-barrel-green-square-packaged/task-431-02-packaged-green-square-doc.md) — doc: record the packaged-build cause and fix in the decal warm-up docs

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-431-01-packaged-green-square-fix]] — step 1, cpp.
- [[task-431-02-packaged-green-square-doc]] — step 2, doc.
- [[task-201-decal-first-frame-green]] — the -game fix (decal warm-up) this build defect escapes.
- [[task-200-decal-green-square-check]] — the first diagnosis of the green square.
- [[barrel]] — the GDD barrel: fuse zone and shockwave ring.
