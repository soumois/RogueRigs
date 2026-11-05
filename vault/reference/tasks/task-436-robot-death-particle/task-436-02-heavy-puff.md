---
id: task-436-02-heavy-puff
description: "Bp: The heavy death puff has the same look about 3 times larger."
parent: task-436-robot-death-particle
kind: task
status: todo
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-436-robot-death-particle]] — parent global task.
