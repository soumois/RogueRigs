---
id: task-031-03-pacing-retune
description: "Pacing bot with a collect profile and a straight-run profile; XP curve tuned so their averaged medians hit L2 0:15, L5 1:10, L10 3:00; shots."
parent: task-031-directional-spawn
kind: task
status: done
---
# Second pacing retune

- goal: early levels come fast for a player who runs as well as for one who collects.
- agent: cpp
- depends_on: task-031-02-spawn-speed-shooter
- inputs: [[xp-level-up]], [[sys-progression]], task-030-02 result (bot rr.Pacing.Run), [[sys-cheats]], [[sys-automation-tests]]
- expected_output: bot profile Run (moves in a straight line, turning only at walls, still aims at the nearest robot, collects only what the magnet takes) next to the existing Collect profile; 3 seeds each to 240 s; XPCurve.csv retuned (simple documented formula) so the mean of the two profiles' medians is within ±20 % of L2 15 s, L5 70 s, L10 180 s, each profile reported too; tests updated; -game shots (open and describe) of the miner running north (robots ahead, sides, few behind) and of the first contact when standing still; proposed GDD curve text. Performance numbers are not valid now (another game runs on this PC): no frame times. From task-032: after task-032-02 put the Blueprint texts in English, remove the French alternatives from Integration.ConsoleExtractionWin and ShuttleTimeoutLoss; run the full headless suite once (task-032-01 did not re-run it after its last fix). From task-032-02: -game shots (open and describe) of the WBP_HUD LocatorArrow pointing to an off-screen console (rr.Debug.Objective Console far away) and of the English HUD texts (horde banner, upgrade screen title, Reroll, end screen).
- done_when: Collect profile medians within ±20 % of L2 15 s, L5 70 s, L10 180 s (owner 2026-10-06: running without collecting is slower by design, the Run profile is only reported); build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 130000
- model: opus
- tokens: input 158, cache write 789562, cache read 7962114, output 72079
- runs: 2
- milestone: M2
- result: Stopped by the owner (2026-10-06): no further retune task. Bot profiles 0 still / 1 collect / 2 run kept. Curve set by the owner: level 1 needs 2 XP, then level² (XPCurve.csv); not measured. Measured on level² before: collect L2 8.3, L5 45.4, L10 110.9 s; run 63.6, 90.1, 200.9 s. Unmeasured candidate curve of the stopped run discarded. Shots: console arrow and English upgrade screen ok. Commits d60beb4, 9108fc7, 79f2c03.

## Relations
- [[task-031-directional-spawn]] — parent global task.
