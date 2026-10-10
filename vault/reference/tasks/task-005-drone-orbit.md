---
id: task-005-drone-orbit
description: "Global task: the drone as a separate C++ actor spawned by the miner, a grey-box sphere circling it; dig state comes with the digging task."
parent: tasks-done-index
kind: task
status: done
---
# Drone (orbit)

- request: "J'aimerais faire le robot (fais une petite sphère pour l'instant) qui tourne autour du joueur et qui va creuser. Voir si tu veux en faire un acteur à part ou un child actor." Owner confirmed a separate actor (2026-10-01).
- goal: in PIE, a small sphere circles the miner at the [[drone]] orbit values and follows it; the drone exposes a state (Orbit now, Dig and Hack later) and reads `MiningSpeed` from the miner's ASC.
- acceptance: (1) `ARRDrone` exists, spawned by `ARRMiner` at `BeginPlay` from an editable class, no ChildActorComponent, no ASC; (2) orbit values are editable data with the [[drone]] defaults; (3) in PIE the drone's location changes over time at about 150 uu from the miner and 150 uu above the floor; (4) `sys-drone` doc and `bp-drone` doc match; lint clean, every subtask committed.
- gdd_check: read [[drone]], [[sys-gas-core]] (drone has no ASC, reads `MiningSpeed`). Orbit values were open: provisional values set in [[drone]]. Dig state, laser (red, owner decision) and block shake belong to the digging task.
- confirmed_by_owner: yes
- milestone: M1
- next_step: none
- result: all acceptance checks passed; the drone orbits at 150 uu, 150 uu high, one turn per 2 s (FollowInterpSpeed 10, provisional). About 92k tokens.

## Subtasks
1. task-005-01-drone-cpp — cpp: `ARRDrone` with orbit state, spawned by the miner.
2. task-005-02-bp-drone — bp: `BP_Drone` sphere, assigned on `BP_Miner`, PIE check.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-005-01-drone-cpp]] — step 1, cpp, drone actor.
- [[task-005-02-bp-drone]] — step 2, bp, drone Blueprint.
