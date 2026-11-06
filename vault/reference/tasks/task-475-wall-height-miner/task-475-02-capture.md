---
id: task-475-02-capture
description: "Capture: Check the raised walls in game."
parent: task-475-wall-height-miner
kind: task
status: done
---
# Walls raised to the miner height: capture

- goal: Check the raised walls in game.
- agent: capture
- depends_on: task-475-01-block-height
- inputs: agent-capture (vault/reference/agents/agent-capture.md), Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-06.webp, Art/Source/cartoon-look/reference/owner-2026-11-05-ingame-light-walls.webp
- expected_output: see done_when
- done_when: C1 wall tops level with the top of the miner standing beside a wall; C2 ore crystals and damaged plates sit on the new tops; C3 a robot right behind a wall is still spotted; C4 barrels and void edge unchanged; sheet before / after sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-475
- usage: included in the executor run of task-475
- runs: 4
- milestone: M2
- started: 2026-11-06T17:29:45Z
- waiting_for: none
- result: 4 launches (Wall475After, Wall475After2, Wall475Before2 with a temporary BlockHeight 100 constructor line, reverted and rebuilt; same commands for before and after: spawner off, peace, BugItGo next to a 2-cell wall, two chasers 400-450 uu behind it, damaged cells, side and front views, FuseBarrel, void edge). C1 seen: wall top level with the head of the miner standing in front of it (Saved/Captures/Wall475Sheets/c1_miner_vs_wall_after.png). C2 seen: ore crystals stand on the new tops, damaged sunk plates on the raised top (c3_crop.png, front_before_after.png). C3 ambiguous, to the owner: at the normal camera (pitch -50, arm 1400) the two chasers 2-3 cells behind the wall show only their upper part, partly behind a crystal cluster; before they were almost whole (c3_crop.png). C4 seen: barrels same size and void edge identical before/after (barrel_void_before_after.png). Sheets for the owner in Saved/Captures/Wall475Sheets/ (game_before_after, c3_crop, front_before_after, side_before_after, barrel_void_before_after, owner_refs_vs_after). No new runtime error in the logs. Not seen: crack mask colour, marked console outline, heal ore, shot impacts on walls.

## Relations
- [[task-475-wall-height-miner]] — parent global task.
