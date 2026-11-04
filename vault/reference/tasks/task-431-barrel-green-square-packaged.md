---
id: task-431-barrel-green-square-packaged
description: "Global task: no green default-decal square at any barrel fuse or explosion in the packaged build, later explosions and later levels included."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Barrel explosion green square in the packaged build

- request: Owner 2026-11-04, verbatim: "J'ai eu le bug du carré vert lors de l'explosion du baril, mais je ne l'ai pas eu la première fois et j'étais en build." (I got the green square bug at the barrel explosion, but not the first time, and I was in a build.) Context: [[task-201-decal-first-frame-green]] removed the one-frame green default-decal box in `-game` runs by warming the decal materials at level start (ARRDecalWarmUp, 2 s); its result says "cooked build and cold cache unverified". The owner now sees the square in a build, not on the first explosion: the warm-up does not cover this case. Bug fix only: no new test, no new effect, game rules unchanged.
- goal: No green default-decal square at the barrel fuse start (danger zone) or at the explosion (shockwave ring) in the packaged build, on every barrel explosion of a run, the first, later ones and those of later levels.
- acceptance: The defect is first reproduced in the staged packaged build (frames and log lines) before any change, or reported not reproduced after 2 runs (then stop, owner wait); the cause is named with evidence; packaged Development build (BuildCookRun of [[build-toolchain]]) exits 0 after the fix; in the staged RogueRigs.exe, frames at fuse start and explosion of at least 5 successive barrel explosions in level 1 (a chain included) and, if a debug command reaches level 2 ([[sys-cheats]]), 2 explosions there, show no green square, otherwise the level 2 check is listed unverified; the task-201 `-game` check stays clean; existing explosion, barrel and telegraph tests pass (only broken expectations updated, no new test); game rules unchanged; committed by path.
- gdd_check: [[barrel]] and [[explosions]] read 2026-11-04: a render defect, no rule change, no `open` value needed, no conflict. Earlier work: [[task-200-decal-green-square-check]] (diagnosis in -game) and [[task-201-decal-first-frame-green]] (warm-up fix, uncooked cause: incomplete shader map, fallback to the engine default deferred decal; cooked build unverified). The packaged-build frames are taken by the cpp subtask itself, as in task-201: capture.js and [[agent-capture]] only launch `-game` from the editor, and the defect is seen in a build.
- confirmed_by_owner: no (plan and questions handed back 2026-11-04)
- milestone: M2
- next_step: task-431-01-packaged-green-square-fix
- started: open
- waiting_for: owner answers Q1 to Q4 (build kind, when the square appeared, its look, a video)
- result: open

## Questions
Asked 2026-11-04, each with a default; open until the owner answers.
- Q1 "En build": the packaged game (RogueRigs.exe from a package or Saved/StagedBuilds), not the editor, PIE or a -game launch. Default: yes, the packaged game.
- Q2 When it appeared: which explosion (the second barrel of the level, a barrel in a chain, a barrel in level 2 to 4 of the run) and at which moment (orange danger zone at fuse start, or shockwave ring at the explosion). Default: not known; the agent reproduces with at least 5 successive explosions in level 1, a chain included, and 2 in level 2 if a debug command reaches it, at fuse start and at the explosion.
- Q3 Its look: like the earlier bug (a pale green box over the zone for one frame, then gone), or it stayed on screen. Default: the same one-frame flash.
- Q4 A video or screenshot of it (C:\Users\soumo\Videos). Default: none; the agent reproduces it itself.

## Subtasks
1. task-431-01-packaged-green-square-fix (vault/reference/tasks/task-431-barrel-green-square-packaged/task-431-01-packaged-green-square-fix.md) — cpp: reproduce the green square in the packaged build, find the cause, fix it, verify in packaged frames
2. task-431-02-packaged-green-square-doc (vault/reference/tasks/task-431-barrel-green-square-packaged/task-431-02-packaged-green-square-doc.md) — doc: record the packaged-build cause and fix in the decal warm-up docs

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-431-01-packaged-green-square-fix]] — step 1, cpp.
- [[task-431-02-packaged-green-square-doc]] — step 2, doc.
- [[task-201-decal-first-frame-green]] — the -game fix (decal warm-up) this build defect escapes.
- [[task-200-decal-green-square-check]] — the first diagnosis of the green square.
- [[barrel]] — the GDD barrel: fuse zone and shockwave ring.
