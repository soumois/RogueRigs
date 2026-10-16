---
id: task-201-decal-first-frame-green
description: "Global task: No green default-material frame when a ground telegraph or explosion shockwave decal appears: decal materials ready before first use, verified in -game frames."
parent: tasks-done-index
kind: task
status: done
---
# Decal first frame green

- request: Code Architect run 2, after the task-200 check: owner answered "Oui corriger les points" (2026-10-16): fix the green square drawn on the first frame of a new decal (barrel danger zone, explosion shockwave) (owner, 2026-10-16) 
- goal: No green default-material frame when a ground telegraph or explosion shockwave decal appears: decal materials ready before first use, verified in -game frames.
- acceptance: build exit 0; explosion, barrel and telegraph tests pass (only broken expectations updated, no new test); the task-200 `-game` command (new prefix) shows no green square at fuse start and at the explosion; the cause found is written in the docs; game rules unchanged; committed by path; committed.
- gdd_check: [[ground-telegraph]], [[explosions]], [[barrel]] read 2026-10-16; a render defect, no rule change, no `open` value.
- confirmed_by_owner: yes (2026-10-16: "Oui corriger les points")
- milestone: M1
- next_step: none
- result: Done. Green default-decal frame removed by warming decal materials at level start (ARRDecalWarmUp); verified in -game frames at fuse start and explosion; cooked build and cold cache unverified. Commits 38fcc48e, 57d10dc1.

## Subtasks
1. task-201-01-decal-first-frame-green — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-197-barrel-explosion-feedback]] — the feature whose frames showed the defect.
- [[task-201-01-decal-first-frame-green]] — step 1, cpp.
