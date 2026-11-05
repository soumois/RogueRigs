---
id: task-446-01-cartoon-burst
description: "Bp: Representative slice: the burst and its material have the cartoon look, in the style of the robot death flash."
parent: task-446-explosions-red
kind: task
status: todo
---
# Barrel and kamikaze explosions in cartoon style: bp

- goal: Representative slice: the burst and its material have the cartoon look, in the style of the robot death flash.
- agent: bp
- depends_on: none
- inputs: task-436-robot-death-particle (vault/reference/tasks/task-436-robot-death-particle.md), bp-robot-death-puff (vault/reference/tech/blueprints/fx/bp-robot-death-puff.md), bp-barrel-explosion (vault/reference/tech/blueprints/fx/bp-barrel-explosion.md), explosions (vault/reference/gdd/terrain/explosions.md), Art/Source/robot-death-fx/20261105-owner-ref-cartoon-explosion.png
- expected_output: see done_when
- done_when: System and material compile with no error; at most 95 sprites read back; one-shot end within 1.5 s; still sized by User.Radius; no pixel of the reference shipped.
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
- [[task-446-explosions-red]] — parent global task.
