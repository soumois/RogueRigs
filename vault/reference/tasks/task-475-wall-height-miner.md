---
id: task-475-wall-height-miner
description: "Global task: Rock, ore and heal ore blocks are raised slightly so their top is at the height of the shrunk miner; play and destruction unchanged."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Walls raised to the miner height

- request: Owner 2026-11-06 (verbatim, answer to the questions of tasks 470-473): "Mur : Q1 : voir image. Rélevé légerement les mur ( il faut que ça fasse la taille du joueur - Mais réduit la taille du joueur de 10 pourcent ). Ok pour tout le reste". Context: the cartoon look request of 2026-11-05 (task-470-cartoon-floor-texture). Two new owner points come out of this answer, one global each: miner shrunk by 10 % (task-474), walls raised to the shrunk miner height (task-475). Confirmed by the owner's own instruction; the provisional choices below were not shown to him and are reported to confirm. This task: "Rélevé légerement les mur ( il faut que ça fasse la taille du joueur [...] )". Owner launch order (relayed by the main session, verbatim): "Lance les task to do dans l'ordre" = launch now; this lifts the earlier "do not launch" instruction.
- goal: In game the walls are slightly higher, their top level with the top of the 10 % smaller miner, and digging, shots and movement work as before.
- acceptance: A1 provisional (owner 2026-11-06, to confirm): the top of every rock, ore and heal ore block is at the height of the miner after task-474 (top of head at rest, tolerance 5 %); old and new heights recorded in the result; A2 the height is one data value tunable in the editor; A3 everything sitting on or in a block follows: ore crystals, heal ore look, crack mask, damaged tilted plates, dig sparks and shot impacts on walls, console marked rock outline; nothing floats or sinks; A4 barrels, the floor, the void edge cliff into space and the void asteroids unchanged; A5 play unchanged: blocks still stop shots and units as before, line-of-sight checks (aim assist, shooters, miner light wall shadows) give the same results, the drone still never flies over walls; A6 at the normal camera view a robot standing right behind a wall stays visible enough to be spotted (the no-occlusion intent of arena-grid stays); A7 compiles, existing tests pass (only expectations broken by this change are updated), no new runtime error in PIE; A8 a -game capture compared with the look before (Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-06.webp) and with Art/Source/cartoon-look/reference/owner-2026-11-05-ingame-light-walls.webp is sent to the owner; the report gives the token cost per subtask and in total.
- gdd_check: Read arena-grid ("Blocks are always low, so nothing hides a unit from the near-vertical camera"), visual-style ("Blocks are always low (no occlusion)"), miner. Conflict decided by the owner himself (his request goes against the vault): blocks rise to the miner height; the no-occlusion intent is kept as acceptance A6; arena-grid and visual-style updated by hand at planning, provisional. Barrels and the void edge are not walls (provisional, to confirm). Order: after task-474 (miner height) and before task-471 (wall look judged on the final geometry) and task-473 (light). No code read.
- confirmed_by_owner: yes (2026-11-06, his own instruction "Mur : Q1 : voir image. Rélevé légerement les mur ( il faut que ça fasse la taille du joueur - Mais réduit la taille du joueur de 10 pourcent ). Ok pour tout le reste"; scope rock, ore, heal ore only is provisional, to confirm)
- milestone: M2
- next_step: task-475-01-block-height
- started: 2026-11-06T17:17:22Z
- waiting_for: none
- executor_tokens: none (no executor run)
- executor_usage: none
- planning_tokens: input 9, cache write 16900, cache read 306744, output 7019
- planning_usage: adacf5eb "Plan cartoon art direction change" 1/6 (tasks 470, 471, 472, 473, 474, 475), units 0.09 M
- result: open

## Subtasks
1. task-475-01-block-height (vault/reference/tasks/task-475-wall-height-miner/task-475-01-block-height.md) — cpp: The terrain block height becomes the shrunk miner height, as one tunable value, with everything on the blocks following.
2. task-475-02-capture (vault/reference/tasks/task-475-wall-height-miner/task-475-02-capture.md) — capture: Check the raised walls in game.
3. task-475-03-doc (vault/reference/tasks/task-475-wall-height-miner/task-475-03-doc.md) — doc: Docs record the new block height.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-475-01-block-height]] — step 1, cpp.
- [[task-475-02-capture]] — step 2, capture.
- [[task-475-03-doc]] — step 3, doc.
