---
id: task-031-03-pacing-retune
description: "Pacing bot with a collect profile and a straight-run profile; XP curve tuned so their averaged medians hit L2 0:15, L5 1:10, L10 3:00; shots."
parent: task-031-directional-spawn
kind: task
status: todo
---
# Second pacing retune

- goal: early levels come fast for a player who runs as well as for one who collects.
- agent: cpp
- depends_on: task-031-02-spawn-speed-shooter
- inputs: [[xp-level-up]], [[sys-progression]], task-030-02 result (bot rr.Pacing.Run), [[sys-cheats]], [[sys-automation-tests]]
- expected_output: bot profile Run (moves in a straight line, turning only at walls, still aims at the nearest robot, collects only what the magnet takes) next to the existing Collect profile; 3 seeds each to 240 s; XPCurve.csv retuned (simple documented formula) so the mean of the two profiles' medians is within ±20 % of L2 15 s, L5 70 s, L10 180 s, each profile reported too; tests updated; -game shots (open and describe) of the miner running north (robots ahead, sides, few behind) and of the first contact when standing still; proposed GDD curve text. Performance numbers are not valid now (another game runs on this PC): no frame times.
- done_when: averaged medians within ±20 %; build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 110000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-031-directional-spawn]] — parent global task.
