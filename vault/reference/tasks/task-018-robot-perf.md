---
id: task-018-robot-perf
description: "Global task: Insights trace, no robot-robot capsule collision with lighter RVO, lighter movement, periodic hitch fix; stress compared after each step."
parent: tasks-index
kind: task
status: in-progress
---
# Robot count optimization

- request: "Sinon que proposes tu pour l'optimisation du nombre de robot ?" Then: "oui lance les deux, comme ça je gagne encore de la marge pour le rendu et j'aimerais en effet que les robots se pack plus, donc ça tombe bien" (2026-10-04).
- goal: more robots per frame and no periodic hitch: 400 robots around 9–10 ms instead of 13.6, no steady-play frame over 25 ms; robots may pack tighter (owner wants it).
- acceptance: (1) build exits 0; all tests pass; (2) each step records before/after S0 and S2 numbers; (3) final S0/S2/S7 table vs [[stress-final-m1]] with the new ceiling; (4) the 61 s hitch explained and removed or reduced; (5) packs still reach the miner and spread around it (integration tests, PIE look by the owner); (6) docs [[perf-risks]], [[sys-enemies]], [[sys-enemy-ai]], new perf node; lint clean; committed.
- gdd_check: Read [[perf-risks]], [[stress-final-m1]], [[enemies-index]]. Owner accepts tighter packing (2026-10-04). Gains are estimates until measured; any step that does not pay off is reverted and reported.
- confirmed_by_owner: yes (2026-10-04, all subtasks pre-approved)
- milestone: M1
- next_step: task-018-03-light-movement
- result: open

## Subtasks
1. task-018-01-trace — cpp: Insights trace at 400 robots, cost breakdown, hitch cause
2. task-018-02-crowd-collision — cpp: robots ignore each other’s capsules, lighter RVO, measure
3. task-018-03-light-movement — cpp: lighter CharacterMovement and distance-based tick, measure
4. task-018-04-hitch — cpp: fix the periodic hitch per the trace, measure
5. task-018-05-final-stress — cpp: final S0, S2, S7 run and comparison

## Relations
- [[tasks-index]] — parent index.
- [[task-018-01-trace]] — step 1, cpp.
- [[task-018-02-crowd-collision]] — step 2, cpp.
- [[task-018-03-light-movement]] — step 3, cpp.
- [[task-018-04-hitch]] — step 4, cpp.
- [[task-018-05-final-stress]] — step 5, cpp.
