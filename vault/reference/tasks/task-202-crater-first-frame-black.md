---
id: task-202-crater-first-frame-black
description: "Global task: No black crater on the frame an explosion opens it: newly exposed floor renders correctly from its first frame, verified in -game frames."
parent: tasks-to-do-index
kind: task
status: todo
---
# Crater first frame black

- request: Code Architect run 2, after the task-200 check: owner answered "Oui corriger les points" (2026-10-16): fix the crater drawn black on the frame the explosion opens it (owner, 2026-10-16)
- goal: No black crater on the frame an explosion opens it: newly exposed floor renders correctly from its first frame, verified in -game frames.
- acceptance: build exit 0; grid, explosion and barrel tests pass (only broken expectations updated, no new test); the task-200 `-game` command (new prefix) shows no black crater on the explosion frame; the cause found is written in the docs; game rules and the 2 ms per-frame destruction budget unchanged; committed by path; committed.
- gdd_check: [[ground-telegraph]], [[explosions]], [[barrel]] read 2026-10-16; a render defect, no rule change, no `open` value.
- confirmed_by_owner: yes (2026-10-16: "Oui corriger les points")
- milestone: M1
- next_step: task-202-01-crater-first-frame-black
- result: open

## Subtasks
1. task-202-01-crater-first-frame-black — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-197-barrel-explosion-feedback]] — the feature whose frames showed the defect.
- [[task-202-01-crater-first-frame-black]] — step 1, cpp.
