---
id: task-446-01-burst-colours
description: "Bp: The explosion burst holds red at the fireball edges and in the smoke."
parent: task-446-explosions-red
kind: task
status: todo
---
# Red in the barrel and kamikaze explosions: bp

- goal: The explosion burst holds red at the fireball edges and in the smoke.
- agent: bp
- depends_on: none
- inputs: bp-barrel-explosion (vault/reference/tech/blueprints/fx/bp-barrel-explosion.md), bp-explosion-fx-barrel (vault/reference/tech/blueprints/fx/bp-explosion-fx-barrel.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), color-code (vault/reference/gdd/art-audio/color-code.md), Art/Source/robot-death-fx/20261105-owner-ref-cartoon-explosion.png
- expected_output: see done_when
- done_when: System compiles with no error; new colour values read back; particle counts, sizes and timings read back unchanged; sphere and ring instances untouched.
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
