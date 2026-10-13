---
id: task-025-06-numbers-visual-check
description: "Screenshots in -game confirm multi-digit numbers, continuous display, crits larger white-gold; S2 perf on vs off."
parent: task-025-damage-numbers
kind: task
status: done
---
# Damage numbers visual recheck

- goal: the owner gets working, readable numbers.
- agent: cpp
- depends_on: task-025-05-bp-numbers-rebuild
- inputs: [[perf-stress-damage-numbers]], [[sys-damage-numbers]], rr.Debug.Shots
- expected_output: -game screenshots over at least 10 s (numbers still appear after 5 s), multi-digit values read correctly, crits larger and white-gold; S2 on vs off at 100/400 robots; results in stress-damage-numbers; keep 2–3 screenshots in Saved/Screenshots and list paths; integration test updated if the contract changed.
- done_when: screenshots confirm the checks; tests pass; lint clean.
- editor_state: closed
- token_ceiling: 80000
- model: opus
- tokens: input 58, cache write 89441, cache read 1720080, output 18926
- runs: 1
- milestone: M2
- result: Screenshots: two-digit numbers read left to right, numbers still appear after 15 s, no double batch; particle dump: digit index 0/1 per number, correct digits, 60 uu spacing, crit 96. Remaining: crits look pale cream (linear 1, 0.8, 0.3 is too light after tonemapping) and neighbouring robots' numbers overlap in crowds → moved to task-027 (gold crit colour, vertical stacking, digit outline). S2 numbers on vs off: no game-thread cost; frames GPU-bound at 22–26 ms (regression vs stress-opt-m1, investigated in 029-01). 7/7 DamageNumbers. New stress-damage-numbers-recheck. Commit caf9c7a.

## Relations
- [[task-025-damage-numbers]] — parent global task.
