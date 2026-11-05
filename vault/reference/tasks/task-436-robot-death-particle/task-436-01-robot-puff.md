---
id: task-436-01-robot-puff
description: "Bp: Representative slice: the robot death puff has its new look."
parent: task-436-robot-death-particle
kind: task
status: in-progress
---
# Better robot death particle: bp

- goal: Representative slice: the robot death puff has its new look.
- agent: bp
- depends_on: none
- inputs: bp-robot-death-puff (vault/reference/tech/blueprints/fx/bp-robot-death-puff.md), enemy-death (vault/reference/gdd/enemies/enemy-death.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), bp-barrel-explosion (vault/reference/tech/blueprints/fx/bp-barrel-explosion.md), Art/Source/robot-death-fx/20261105-owner-ref-cartoon-explosion.png
- expected_output: see done_when
- done_when: System compiles with no error, one-shot behaviour and pool return kept, at most 20 sprites read back, material compiles with flat cel-shaded bands, colours as in the reference, red included, no pixel of the reference shipped.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-05T17:37:13Z
- waiting_for: none
- result: open

## Relations
- [[task-436-robot-death-particle]] — parent global task.
