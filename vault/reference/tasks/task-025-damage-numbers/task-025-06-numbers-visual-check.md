---
id: task-025-06-numbers-visual-check
description: "Screenshots in -game confirm multi-digit numbers, continuous display, crits larger white-gold; S2 perf on vs off."
parent: task-025-damage-numbers
kind: task
status: in-progress
---
# Damage numbers visual recheck

- goal: the owner gets working, readable numbers.
- agent: cpp
- depends_on: task-025-05-bp-numbers-rebuild
- inputs: [[stress-damage-numbers]], [[sys-damage-numbers]], rr.Debug.Shots
- expected_output: -game screenshots over at least 10 s (numbers still appear after 5 s), multi-digit values read correctly, crits larger and white-gold; S2 on vs off at 100/400 robots; results in stress-damage-numbers; keep 2–3 screenshots in Saved/Screenshots and list paths; integration test updated if the contract changed.
- done_when: screenshots confirm the checks; tests pass; lint clean.
- editor_state: closed
- token_ceiling: 80000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-025-damage-numbers]] — parent global task.
