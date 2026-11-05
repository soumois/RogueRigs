---
id: task-438-02-debris-system
description: "Bp: The one-shot dig debris Niagara system exists and is the one the game plays."
parent: task-438-dig-debris
kind: task
status: todo
---
# Debris when digging: bp

- goal: The one-shot dig debris Niagara system exists and is the one the game plays.
- agent: bp
- depends_on: task-438-01-trigger-on-break
- inputs: bp-dig-impact (vault/reference/tech/blueprints/fx/bp-dig-impact.md), color-code (vault/reference/gdd/art-audio/color-code.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md)
- expected_output: see done_when
- done_when: System compiles with no error, one-shot, at most 12 sprites and 0.8 s read back, colour from the parameter the code sets.
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
- [[task-438-dig-debris]] — parent global task.
