---
id: task-305-console-hack-10s
description: "Global task: The console hack takes 10 s instead of 12 s; the bonus chest is unchanged."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Console hack time 10 s

- request: "Remet aussi à 10 sec le temps pour prendre la zone de piratage." (owner, 2026-10-23)
- goal: The console hack takes 10 s instead of 12 s; the bonus chest is unchanged.
- acceptance: ARRConsole HackDuration default 10 (BP_ObjectiveConsole does not override it); ARRBonusChest stays 8; GDD objective-console and tech doc say 10 s, provisional owner 2026-10-23; only tests this change breaks are updated; committed.
- gdd_check: objective-console read (2026-10-23): hack time 12 s (owner 2026-10-13, provisional) -> 10 s, console only, chest unchanged (owner 2026-10-23).
- confirmed_by_owner: yes (2026-10-23, "ok" to the plan and every default; planned in one planning pass, each task run later by its own orchestrator)
- milestone: M2
- next_step: task-305-01-console-hack-10s
- result: open

## Subtasks
1. task-305-01-console-hack-10s — cpp: Console HackDuration 10 s

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-305-01-console-hack-10s]] — step 01, cpp.
