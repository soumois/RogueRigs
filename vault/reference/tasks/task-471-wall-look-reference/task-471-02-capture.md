---
id: task-471-02-capture
description: "Capture: Final independent check of the walls against image 2."
parent: task-471-wall-look-reference
kind: task
status: done
---
# Walls look like the owner in-game reference: capture

- goal: Final independent check of the walls against image 2.
- agent: capture
- depends_on: task-471-01-wall-match-loop
- inputs: agent-capture (vault/reference/agents/agent-capture.md), Art/Source/cartoon-look/reference/owner-2026-11-05-ingame-light-walls.webp, Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-06.webp
- expected_output: see done_when
- done_when: C1 lilac-violet wall tops lighter than the floor; C2 thin glowing orange rim along the wall top edges, also far from the miner; C3 dark stratified sides; C4 ore crystals and heal ore keep their colours; C5 a damaged wall still shows its cracks or tilted plates; C6 the rim does not read as a barrel; side-by-side sheets before / after / image 2 sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-471
- usage: included in the executor run of task-471
- runs: 1
- milestone: M2
- started: 2026-11-06T17:51:01Z
- waiting_for: none
- result: 1 launch (Saved/Captures/Wall471Final, 1920x1080 -game, spawner off, peace, damaged cells 105,147/148, FuseBarrel, far and void views). C1 seen: lilac-violet tops lighter than the magenta floor (game_00, spawn_00). C2 seen: thin orange line along every wall lip, also on the far plateaus (far_00, barrel_00). C3 seen: dark purple sides with horizontal strata (game_00, front_00). C4 seen for ore (violet crystals unchanged); heal ore ambiguous: small red patches on far plateaus (far_00 left) not identifiable at that size. C5 seen: damaged wall shows crack lines and sunk plates; their seams now also carry thin orange lines (Wall471Sheets/final_damaged_crop.png), to the owner. C6 seen: barrels stay full orange cylinders in their yellow telegraph, the rim is a thin line (final_barrel_heal.png). No shader or material error in run.log. Sheets for the owner: Saved/Captures/Wall471Sheets/r3_before_after_target.png, r3_spawn_vs_target.png, r1_r2_r3_rounds.png, final_damaged_crop.png, final_barrel_heal.png.

## Relations
- [[task-471-wall-look-reference]] — parent global task.
