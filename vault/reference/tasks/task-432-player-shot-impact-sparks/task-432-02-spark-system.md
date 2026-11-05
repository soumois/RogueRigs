---
id: task-432-02-spark-system
description: "Bp: The white-gold impact spark Niagara system exists and is the one the game spawns."
parent: task-432-player-shot-impact-sparks
kind: task
status: in-progress
---
# Player shot impact sparks: bp

- goal: The white-gold impact spark Niagara system exists and is the one the game spawns.
- agent: bp
- depends_on: task-432-01-spawn-on-hit
- inputs: color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), bp-dig-impact (vault/reference/tech/blueprints/fx/bp-dig-impact.md), bp-muzzle-flash (vault/reference/tech/blueprints/fx/bp-muzzle-flash.md)
- expected_output: see done_when
- done_when: One-shot CPU sprite system, at most 8 sprites and 0.25 s, white-gold, no compile or stack error, values read back, assigned so the game path of the previous subtask plays it.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-05T15:56:21Z
- waiting_for: none
- result: open

## Relations
- [[task-432-player-shot-impact-sparks]] — parent global task.
