---
id: task-018-03-light-movement
description: "Lighter CharacterMovement for robots on the flat floor and a slower movement tick far from the miner; stress before and after."
parent: task-018-robot-perf
kind: task
status: todo
---
# Light movement

- goal: cut the per-robot movement cost, the main game-thread cost per the trace (CharacterMovement 5.9 ms S0 / 7.0 ms S2 at 400: walking physics 4.65, move sweeps 2.15, floor check 1.86, attached mesh parts update 0.81). Also consider cheaper updates of the robots’ attached rigid parts (e.g. fewer attached components, no overlaps on them, transform updates deferred).
- agent: cpp
- depends_on: task-018-02-crowd-collision
- inputs: task-018-01 and 018-02 results, [[perf-risks]], [[sys-enemies]], [[sys-enemy-ai]], [[sys-automation-tests]], [[cpp-standard]].
- expected_output: robot CharacterMovement settings for a flat arena (no physics interaction, cheaper floor checks, no unneeded sub-stepping), and tick interval of movement/AI raised for robots far from the miner (distance thresholds documented) without visible stutter near the screen; shotgun knockback still works; stress before/after; tests pass. Docs updated. Revert any setting that does not pay off and report it.
- done_when: build exits 0; all `RogueRigs.*` tests pass headless; before/after stress numbers (S0 and S2 at 100, 200, 400, same procedure as [[stress-final-m1]]) recorded; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 90000
- milestone: M1
- result: open

## Relations
- [[task-018-robot-perf]] — parent global task.
