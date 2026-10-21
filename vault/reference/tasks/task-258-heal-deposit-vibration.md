---
id: task-258-heal-deposit-vibration
description: "Global task: The heal deposit no longer vibrates by itself: only its upper part moves without contact, even in the editor; find the cause and fix it."
parent: tasks-done-index
kind: task
status: done
---
# Heal deposit vibration

- request: the heal ore block vibrates (only its upper part) without being touched, even in the editor (owner, 2026-10-20)
- goal: The heal deposit no longer vibrates by itself: only its upper part moves without contact, even in the editor; find the cause and fix it.
- acceptance: The heal deposit stays still when untouched (cause identified with numbers: which data index or node moves it), the dig shake still works, existing RogueRigs.Dig, RogueRigs.Ore, RogueRigs.PulseDesync, RogueRigs.Integration.FineContactDigOpensPassage and RogueRigs.RunFlow tests pass; docs updated; lint clean. If the cause is in a Blueprint or material asset, report it for a bp subtask instead of editing it; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-20).
- confirmed_by_owner: yes (2026-10-20)
- milestone: M1
- next_step: none
- result: Done. Cause: M_RockSlab (rock slot of the heal deposit) reads per-instance data 4..7 as its shake quadrants; task-257-02 wrote the pulse rate (0.8-1.2) at index 5, so 5 of 12 heal deposits (rate >= 1) shook on their +X side forever, also in the editor. Fix in C++: ISMs with a rock slot get 4 floats and no pulse data (rock reads 0, still), Duskite unchanged; check reads 12 heal instances, shake input 0.0000 on all; 27 of 27 tests pass. Cost: heal crystals lose the hashed pulse (position term only) until M_DepositCrystalGlow reads indices 13 and 14 and C++ writes them (bp + cpp follow-up). Not seen on screen.

## Subtasks
1. task-258-01-heal-deposit-vibration — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-258-01-heal-deposit-vibration]] — step 1, cpp.
