---
id: task-025-03-numbers-perf
description: "Integration test with the final Niagara asset and a -game S2 measure at 400 robots compared with the previous damage text."
parent: task-025-damage-numbers
kind: task
status: done
---
# Damage numbers check

- goal: the new numbers are correct and cheap.
- agent: cpp
- depends_on: task-025-02-bp-numbers
- inputs: [[perf-stress-opt-m1]], [[sys-stress]], task-025-02 result.
- expected_output: VISUAL CHECK first (the bp agent could not see the result): run -game on L_ArenaM1 with robots being hit (stress fire mode or cheats), take screenshots with HighResShot or the screenshot console command at a few moments, open the PNG files and confirm digits appear above hit robots, rise and fade, crits larger and white-gold, numbers readable; if wrong, report precisely what is wrong (do not edit the Niagara asset; describe the fix for the bp agent); also check the persistent Niagara component exists and receives NewCount > 0 in an integration test; integration test (hits produce merged entries, crits flagged); S2 -game at 100/400 robots before/after numbers in stress-opt-m1 or a short new node; full run.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 70000
- model: opus
- tokens: input 138, cache write 175077, cache read 7656715, output 54626
- runs: 2
- milestone: M2
- result: Screenshot check in -game found NS_DamageNumbers defects: emitter stops after its first loop (nothing after about 1 s), Engine.ExecutionIndex reads 0 in Particle Spawn (only the first digit of the first number, stacked), 4 particles per number instead of 5, about 1 batch in 4 spawned twice (count reset), crit colour pale; digits do render from the atlas, rise, fade, crits larger. Subsystem now creates its own component (headless-safe), restarts it on hit, GetTotalPushedCount, GetParticleStats; rr.Debug.Shots and rr.Debug.CritChance. Integration test NiagaraIntegration; 147/147. S2 game time unchanged on vs off (100: 8.20/8.21; 400: 15.49/16.09). New stress-damage-numbers. Commit 51adc31. Follow-up subtasks 025-04 to 025-06 added (per-digit contract).

## Relations
- [[task-025-damage-numbers]] — parent global task.
