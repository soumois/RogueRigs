---
id: task-436-02-heavy-puff
description: "Bp: The heavy death puff has the same look about 3 times larger."
parent: task-436-robot-death-particle
kind: task
status: done
---
# Better robot death particle: bp

- goal: The heavy death puff has the same look about 3 times larger.
- agent: bp
- depends_on: task-436-01-robot-puff
- inputs: bp-heavy-death-puff (vault/reference/tech/blueprints/fx/bp-heavy-death-puff.md), bp-robot-death-puff (vault/reference/tech/blueprints/fx/bp-robot-death-puff.md)
- expected_output: see done_when
- done_when: System compiles with no error, values read back, same one-shot behaviour.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-436
- usage: included in the executor run of task-436
- runs: 1
- milestone: M2
- started: 2026-11-05T17:49:16Z
- waiting_for: none
- result: NS_HeavyDeathPuff rebuilt from the same script at k = 3: sprite sizes, ring radii and speeds x3, position offset z +130 kept, same 18 sprites, timing and one-shot settings read back; compile state UpToDate, 0 errors, 0 warnings. Look unverified in game (A7).

## Relations
- [[task-436-robot-death-particle]] — parent global task.
