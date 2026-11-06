---
id: task-474-miner-scale-down
description: "Global task: The miner is shown 10 % smaller; collision and gameplay values unchanged; everything attached to it follows the new size."
parent: tasks-to-do-index
kind: task
status: todo
---
# Miner 10 % smaller

- request: Owner 2026-11-06 (verbatim, answer to the questions of tasks 470-473): "Mur : Q1 : voir image. Rélevé légerement les mur ( il faut que ça fasse la taille du joueur - Mais réduit la taille du joueur de 10 pourcent ). Ok pour tout le reste". Context: the cartoon look request of 2026-11-05 (task-470-cartoon-floor-texture). Two new owner points come out of this answer, one global each: miner shrunk by 10 % (task-474), walls raised to the shrunk miner height (task-475). Confirmed by the owner's own instruction; the provisional choices below were not shown to him and are reported to confirm. This task: "réduit la taille du joueur de 10 pourcent". Owner launch order (relayed by the main session, verbatim): "Lance les task to do dans l'ordre" = launch now; this lifts the earlier "do not launch" instruction.
- goal: In game the miner is 10 % smaller than now, everything attached to it still fits, and gameplay is unchanged.
- acceptance: A1 the miner mesh is 90 % of its current size in every direction (height measured top of head at rest, tolerance 1 %), recorded before and after in the result; A2 provisional (owner 2026-11-06, rules versus rendering, to confirm): only the look shrinks; the collision capsule, movement speed, pickup radius, weapon ranges and every gameplay value are unchanged; A3 everything attached to the miner follows the new size: rifle in the hand, left-hand IK on the handguard, muzzle position, aim laser start, muzzle flash, damage, heal and ore numbers above the head, hit and heal effects; nothing floats or clips; A4 the camera, the drone and its size are unchanged; A5 the miner light height set by the owner in BP_Miner is not changed; A6 animations play as before (no foot sliding added, upper rifle pose intact); A7 compiles, existing tests pass (only expectations broken by this change are updated), no new runtime error in PIE; A8 a -game capture compared with the look before (Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-06.webp) is sent to the owner; the report gives the token cost per subtask and in total.
- gdd_check: Read miner (look, asset, sockets, IK), visual-style, art-style-guide A4 (drone about the size of the miner head and shoulders: drone unchanged by default, ratio shifts slightly). No GDD value for the miner size: decided by the owner, 90 % of now. Rules versus rendering (orchestrator-lessons): collision and gameplay values unchanged, provisional, reported to the owner to confirm. Order: runs before task-475 (wall height measured on the shrunk miner) and before task-472 (shadow size from the footprint). No code read.
- confirmed_by_owner: yes (2026-11-06, his own instruction "Mur : Q1 : voir image. Rélevé légerement les mur ( il faut que ça fasse la taille du joueur - Mais réduit la taille du joueur de 10 pourcent ). Ok pour tout le reste"; A2 hitbox and gameplay unchanged is provisional, to confirm)
- milestone: M2
- next_step: task-474-01-miner-scale
- started: open
- waiting_for: none
- executor_tokens: none (no executor run)
- executor_usage: none
- planning_tokens: input 9, cache write 16900, cache read 306744, output 7019
- planning_usage: adacf5eb "Plan cartoon art direction change" 1/6 (tasks 470, 471, 472, 473, 474, 475), units 0.09 M
- result: open

## Subtasks
1. task-474-01-miner-scale (vault/reference/tasks/task-474-miner-scale-down/task-474-01-miner-scale.md) — bp: The miner mesh is shown at 90 % with every attachment and effect following, collision and gameplay unchanged.
2. task-474-02-capture (vault/reference/tasks/task-474-miner-scale-down/task-474-02-capture.md) — capture: Check the smaller miner in game.
3. task-474-03-doc (vault/reference/tasks/task-474-miner-scale-down/task-474-03-doc.md) — doc: Docs record the new miner scale.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-474-01-miner-scale]] — step 1, bp.
- [[task-474-02-capture]] — step 2, capture.
- [[task-474-03-doc]] — step 3, doc.
