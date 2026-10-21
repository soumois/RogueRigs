---
id: task-258-heal-deposit-vibration
description: "Global task: The heal deposit no longer vibrates by itself: only its upper part moves without contact, even in the editor; find the cause and fix it."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Heal deposit vibration

- request: the heal ore block vibrates (only its upper part) without being touched, even in the editor (owner, 2026-10-20)
- goal: The heal deposit no longer vibrates by itself: only its upper part moves without contact, even in the editor; find the cause and fix it.
- acceptance: The heal deposit stays still when untouched (cause identified with numbers: which data index or node moves it), the dig shake still works, existing RogueRigs.Dig, RogueRigs.Ore, RogueRigs.PulseDesync, RogueRigs.Integration.FineContactDigOpensPassage and RogueRigs.RunFlow tests pass; docs updated; lint clean. If the cause is in a Blueprint or material asset, report it for a bp subtask instead of editing it; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-20).
- confirmed_by_owner: yes (2026-10-20)
- milestone: M1
- next_step: task-258-01-heal-deposit-vibration
- result: open

## Subtasks
1. task-258-01-heal-deposit-vibration — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-258-01-heal-deposit-vibration]] — step 1, cpp.
