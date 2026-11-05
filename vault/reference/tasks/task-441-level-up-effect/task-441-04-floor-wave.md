---
id: task-441-04-floor-wave
description: "Bp: The one-shot gold floor wave Niagara system exists, sized to the push radius."
parent: task-441-level-up-effect
kind: task
status: todo
---
# Level-up effect rework: bp

- goal: The one-shot gold floor wave Niagara system exists, sized to the push radius.
- agent: bp
- depends_on: task-441-03-robot-push
- inputs: bp-explosion-shockwave (vault/reference/tech/blueprints/fx/bp-explosion-shockwave.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md)
- expected_output: see done_when
- done_when: System compiles with no error, one-shot, radius read back about 300 uu, bounded CPU sprites.
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
- [[task-441-level-up-effect]] — parent global task.
