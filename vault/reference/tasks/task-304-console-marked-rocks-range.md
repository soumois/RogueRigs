---
id: task-304-console-marked-rocks-range
description: "Global task: The console lands only where its zone holds between 10 and 40 markable blocks, with both bounds editable in Blueprint defaults."
parent: tasks-done-index
kind: task
status: done
---
# Console marked rocks between 10 and 40

- request: "Il faut aussi un nombre de dalle minimum à miner (10) et un nombre maximal (40). Qu'on ait un contrôle là dessus." (owner, 2026-10-23)
- goal: The console lands only where its zone holds between 10 and 40 markable blocks, with both bounds editable in Blueprint defaults.
- acceptance: drop picker keeps only spots whose zone holds MinMarkedCells 10 to MaxMarkedCells 40 markable blocks; no fitting spot: the spot with the nearest count, logged as a warning; both EditDefaultsOnly on the console; the bonus chest (no marked rocks) unaffected; GDD and tech doc; committed.
- gdd_check: objective-console read (2026-10-23): no marked-rock count rule before; owner default: 10..40 (provisional owner 2026-10-23), replaces the plain marked-rock preference; fallback nearest count with a warning.
- confirmed_by_owner: yes (2026-10-23, "ok" to the plan and every default; planned in one planning pass, each task run later by its own orchestrator)
- milestone: M2
- next_step: none
- executor_tokens: input 24, cache write 77277, cache read 777037, output 5125
- executor_usage: runs 1, turns 12, context start 51k peak 82k, resumes 0, 12 min, units 0.20 M
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Done. ARRConsole MinMarkedCells 10 / MaxMarkedCells 40 (EditDefaultsOnly, provisional owner 2026-10-23); console picker keeps spots in range, else nearest count with a warning; chest unfiltered; GDD objective-console, sys-objectives-console-chest, sys-objectives-run-flow, bp-objective-console updated; build green; Console.* and RunFlow.* 7/7 pass, no test changed; code 0a357aea, docs 3134b212

## Subtasks
1. task-304-01-console-marked-rocks-range — cpp: Drop picker marked-rock range

## Relations
- [[tasks-done-index]] — parent index.
- [[task-304-01-console-marked-rocks-range]] — step 01, cpp.
