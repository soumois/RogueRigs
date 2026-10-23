---
id: task-305-console-hack-10s
description: "Global task: The console hack takes 10 s instead of 12 s; the bonus chest is unchanged."
parent: tasks-done-index
kind: task
status: done
---
# Console hack time 10 s

- request: "Remet aussi à 10 sec le temps pour prendre la zone de piratage." (owner, 2026-10-23)
- goal: The console hack takes 10 s instead of 12 s; the bonus chest is unchanged.
- acceptance: ARRConsole HackDuration default 10 (BP_ObjectiveConsole does not override it); ARRBonusChest stays 8; GDD objective-console and tech doc say 10 s, provisional owner 2026-10-23; only tests this change breaks are updated; committed.
- gdd_check: objective-console read (2026-10-23): hack time 12 s (owner 2026-10-13, provisional) -> 10 s, console only, chest unchanged (owner 2026-10-23).
- confirmed_by_owner: yes (2026-10-23, "ok" to the plan and every default; planned in one planning pass, each task run later by its own orchestrator)
- milestone: M2
- next_step: none
- result: Done. ARRConsole HackDuration 12 -> 10 s (RRConsole.h, commit 4070fb23); chest stays 8 s; BP_ObjectiveConsole overrides nothing (doc, asset not opened). Tests fixed: Console.HackRules (RRRunFlowTests.cpp) and Console.ZoneTelegraph (RRConsoleTelegraphTests.cpp) timings. GDD objective-console and sys-objectives-console-chest updated (commit 0131474c). Build exit 0; 8/8 tests pass (Console.*, RunFlow.*, Integration.ConsoleExtractionWin).

## Subtasks
1. task-305-01-console-hack-10s — cpp: Console HackDuration 10 s

## Relations
- [[tasks-done-index]] — parent index.
- [[task-305-01-console-hack-10s]] — step 01, cpp.
