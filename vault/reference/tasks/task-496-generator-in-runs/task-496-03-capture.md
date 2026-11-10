---
id: task-496-03-capture
description: "Capture: One -game capture of the generated terrains of level 1 and level 2, compared with the hand layouts before."
parent: task-496-generator-in-runs
kind: task
status: done
---
# Layout generator wired into real runs: capture

- goal: One -game capture of the generated terrains of level 1 and level 2, compared with the hand layouts before.
- agent: capture
- depends_on: task-496-02-request-timing
- inputs: agent-capture (vault/reference/agents/agent-capture.md), arena-grid (vault/reference/gdd/terrain/arena-grid.md)
- expected_output: see done_when
- done_when: C1 Level 1 seen after New Run from the main menu: a full arena, irregular asteroid edge, rock, ore, heal ore and barrels, different from arena_m1_fine before. C2 Level 2 seen after Continue: a full arena different from arena_l2 before. C3 The loading screen and the fade in show no frozen or empty arena. Sheet sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: not measured: record-cost.js found no usage for this executor run (no transcript folder for this repo path)
- usage: included in the executor run of task-496 (not measurable)
- runs: 1
- milestone: M2
- started: 2026-11-10T20:16:14Z
- waiting_for: none
- result: Done with one point for the owner. 3 launches (capture.js, map none -> main menu -> New Run), 6 images opened. C1 SEEN (Saved/Captures/task496_terrains2/l1full_00.png, l1mid_00.png): full generated level 1 after New Run from the menu, irregular asteroid edge, rock with ore crystals, floor network, barrels (orange cylinders against walls in l1mid and Saved/Captures/task496_heal/q1_00.png), different from arena_m1_fine before (l1hand_00.png, loaded by rr.Level.LoadLayout in the same run and camera). Heal ore AMBIGUOUS: the log counts 56 heal_ore cells, but at the overview and quadrant scales (task496_heal/q1-q4) heal veins are not told apart from robots for sure (pink spots against walls): left to the owner. C2 SEEN (l2full_00.png vs l2hand_00.png): full generated level 2 after Stats.Next + Continue, different from arena_l2. C3 SEEN (task496_terrains/load_00..05.png, task496_terrains2/l2fade_00..03.png): New Run loading screen 'DROPPING IN Level 1', one black frame (fade start), then the arena with HUD; level 2 Continue fade path: cards screen then the level 2 arena, no frozen or empty arena. Owner sheet: Saved/Captures/task496_terrains2/owner_sheet.png. No defect.

## Relations
- [[task-496-generator-in-runs]] — parent global task.
