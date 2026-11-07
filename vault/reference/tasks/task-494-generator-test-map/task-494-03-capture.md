---
id: task-494-03-capture
description: "Capture: Visual check of the test map and of one reload."
parent: task-494-generator-test-map
kind: task
status: done
---
# Enemy-free test map with a command to generate and reload a new level: capture

- goal: Visual check of the test map and of one reload.
- agent: capture
- depends_on: task-494-02-test-map
- inputs: agent-capture (vault/reference/agents/agent-capture.md)
- expected_output: see done_when
- done_when: C1 the test map shows a generated asteroid with rock, ore, heal veins and barrels, miner at the spawn; C2 no robot, console, chest or shuttle visible after 2 minutes; C3 the seed is shown on screen; C4 after rr.Level.New a different layout is visible with its new seed and the miner at the new spawn; C5 after rr.Level.New with the first seed the first layout is back. Frames sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-494
- usage: included in the executor run of task-494
- runs: 2
- milestone: M2
- started: 2026-11-07T08:37:42Z
- waiting_for: none
- result: Done. 2 launches (Saved/Captures/task494_capture, task494_capture2), L_GenTest -game 1600x900, camera rr.Debug.Camera -70 5200 for the wide frames. Launch 1 showed a defect: after rr.Level.New the minimap disc went black (second URRMinimap texture created with a name already used in the transient package, overwriting the texture the widget material showed; log 'Gamethread hitch waiting for resource cleanup ... MinimapTexture_0'). One focused fix (commit dc57cc4a, RRMinimap.cpp: MakeUniqueObjectName on GetTransientPackage()), launch 2 passes. Verdicts on launch 2: C1 seen (c1a_00, c1b_00, crops_c1.png: rock slabs, ore crystals, red heal veins, orange barrel, miner and drone at the centre spawn); C2 seen (c2a_00 at ~125 s: none visible; log watch lines 3 s to 161 s robots 0, consoles 0, bonus chests 0, shuttles 0, run flow off); C3 seen (top-centre line 'Seed 781214 (rr.Level.New [seed] for a new layout)'); C4 seen (c4a_00: other layout, 'Seed 4327', miner at the spawn; c4b_00 random seed 606314, third layout); C5 seen in this form: rr.Level.New 4321 after another layout gives back the 4327 layout identical to c4a (c5a_00; same JSON crc 45dc136a in both runs). The very first layout of a session uses a random seed unknown before the run, so C5 was checked with the first seed given by command, not the start seed. Timing request to playable: 3.3-4.7 s, worst 6.9 s when the search tried 14 seeds (launch 1).

## Relations
- [[task-494-generator-test-map]] — parent global task.
