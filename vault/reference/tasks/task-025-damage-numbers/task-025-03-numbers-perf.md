---
id: task-025-03-numbers-perf
description: "Integration test with the final Niagara asset and a -game S2 measure at 400 robots compared with the previous damage text."
parent: task-025-damage-numbers
kind: task
status: in-progress
---
# Damage numbers check

- goal: the new numbers are correct and cheap.
- agent: cpp
- depends_on: task-025-02-bp-numbers
- inputs: [[stress-opt-m1]], [[sys-stress]], task-025-02 result.
- expected_output: VISUAL CHECK first (the bp agent could not see the result): run -game on L_ArenaM1 with robots being hit (stress fire mode or cheats), take screenshots with HighResShot or the screenshot console command at a few moments, open the PNG files and confirm digits appear above hit robots, rise and fade, crits larger and white-gold, numbers readable; if wrong, report precisely what is wrong (do not edit the Niagara asset; describe the fix for the bp agent); also check the persistent Niagara component exists and receives NewCount > 0 in an integration test; integration test (hits produce merged entries, crits flagged); S2 -game at 100/400 robots before/after numbers in stress-opt-m1 or a short new node; full run.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 70000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-025-damage-numbers]] — parent global task.
