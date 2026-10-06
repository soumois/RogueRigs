---
id: task-031-03-pacing-retune
description: "Pacing bot with a collect profile and a straight-run profile; XP curve tuned so their averaged medians hit L2 0:15, L5 1:10, L10 3:00; shots."
parent: task-031-directional-spawn
kind: task
status: in-progress
---
# Second pacing retune

- goal: early levels come fast for a player who runs as well as for one who collects.
- agent: cpp
- depends_on: task-031-02-spawn-speed-shooter
- inputs: [[xp-level-up]], [[sys-progression]], task-030-02 result (bot rr.Pacing.Run), [[sys-cheats]], [[sys-automation-tests]]
- expected_output: bot profile Run (moves in a straight line, turning only at walls, still aims at the nearest robot, collects only what the magnet takes) next to the existing Collect profile; 3 seeds each to 240 s; XPCurve.csv retuned (simple documented formula) so the mean of the two profiles' medians is within ±20 % of L2 15 s, L5 70 s, L10 180 s, each profile reported too; tests updated; -game shots (open and describe) of the miner running north (robots ahead, sides, few behind) and of the first contact when standing still; proposed GDD curve text. Performance numbers are not valid now (another game runs on this PC): no frame times. From task-032: after task-032-02 put the Blueprint texts in English, remove the French alternatives from Integration.ConsoleExtractionWin and ShuttleTimeoutLoss; run the full headless suite once (task-032-01 did not re-run it after its last fix). From task-032-02: -game shots (open and describe) of the WBP_HUD LocatorArrow pointing to an off-screen console (rr.Debug.Objective Console far away) and of the English HUD texts (horde banner, upgrade screen title, Reroll, end screen).
- done_when: averaged medians within ±20 %; build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 130000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: Bot profiles 0 still / 1 collect / 2 run (straight, turns at walls). XPCurve need(L) = L². Medians collect L2 8.3, L5 45.4, L10 110.9; run 63.6, 90.1, 200.9; mean L2 36.0 (fail, unreachable by the curve: runner first XP 19–80 s), L5 67.8, L10 155.9 (pass). Profiles differ 3.4× in XP. Tests follow the curve, French alternatives removed; build 0; full suite 194/195, the failing JuiceKillDropsStreamingCubes fixed and its group passes. Shots: console arrow and English upgrade screen ok; first contact still at about 3.5 s ok; north run, horde banner and end screen not captured. See sys-progression.

## Relations
- [[task-031-directional-spawn]] — parent global task.
