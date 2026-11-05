---
id: task-439-02-debris-system
description: "Bp: The wall debris Niagara system exists, flies outward from the centre it is given, and is the one the game plays."
parent: task-439-explosion-wall-debris
kind: task
status: in-progress
---
# Wall debris thrown away from explosions: bp

- goal: The wall debris Niagara system exists, flies outward from the centre it is given, and is the one the game plays.
- agent: bp
- depends_on: task-439-01-debris-request
- inputs: bp-barrel-explosion (vault/reference/tech/blueprints/fx/bp-barrel-explosion.md), color-code (vault/reference/gdd/art-audio/color-code.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md)
- expected_output: see done_when
- done_when: System compiles with no error, one-shot, direction read from the given centre, at most 60 particles and 1.5 s read back.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-05T19:30:19Z
- waiting_for: none
- result: open

## Relations
- [[task-439-explosion-wall-debris]] — parent global task.
