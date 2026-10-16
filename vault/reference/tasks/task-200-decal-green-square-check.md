---
id: task-200-decal-green-square-check
description: "Global task: second -game run of a barrel fuse and explosion to see whether the green decal square returns once shaders are cached."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Decal green square check

- request: Code Architect run 2 follow-up of task-197: owner answered "Ok" to the proposals (2026-10-16): second run to confirm the green square on the first decal frame is the shader compile, nothing fixed if it is (owner, 2026-10-16)
- goal: Second -game run of the barrel fuse and explosion: does the green square return on the first decal frame once shaders are cached? Report only.
- acceptance: frames at the fuse start and the explosion exist and were looked at; the report says whether the green square appears and its likely cause; nothing committed by the agent; committed.
- gdd_check: [[barrel]] owner rule 2026-10-16 (ring and burst provisional), [[explosions]] read 2026-10-16; no conflict, no `open` value.
- confirmed_by_owner: yes (2026-10-16: "Ok" to the proposals)
- milestone: M1
- next_step: task-200-01-decal-green-square-check
- result: open

## Subtasks
1. task-200-01-decal-green-square-check — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[barrel]] — the owner rule it refines.
- [[task-200-01-decal-green-square-check]] — step 1, cpp.
