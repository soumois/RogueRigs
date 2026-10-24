---
id: task-313-drone-at-console-socket
description: "Global task: While hacking, the drone hovers in front of the console data socket, facing it, and the hack link aims at it."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Drone at the console socket while hacking

- request: "Le robot doit se mettre prêt de la zone où il est censé se clipser pour pirater la console." (owner, 2026-10-24)
- goal: While hacking, the drone hovers in front of the console round data socket, at its height, facing it, and the hack link aims at it.
- acceptance: during the console hack the drone hovers in front of the round data socket on the front console block, at its height, facing it; the hack link aims at the socket; if that point is blocked the existing rule applies (comes back toward the miner); leaving the zone unchanged; socket position follows the scaled mesh of task-311; -game capture for the owner; committed.
- gdd_check: drone and objective-console read (2026-10-24): rendering and position only, the hack rules are unchanged; no conflict, no open value. Decision 13 (owner 2026-10-24). Depends on task-311 (the socket position comes from the scaled mesh).
- confirmed_by_owner: yes (2026-10-24, "ok" to the plan and every default; planned in one planning pass, each task run later by its own orchestrator)
- milestone: M2
- next_step: task-313-02-capture-drone-socket
- result: open

## Subtasks
1. task-313-01-drone-socket-hover — cpp: Drone hover point at the socket
2. task-313-02-capture-drone-socket — cpp: Capture the drone at the socket

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-313-01-drone-socket-hover]] — step 01, cpp.
- [[task-313-02-capture-drone-socket]] — step 02, cpp.
