---
id: task-441-04-floor-wave
description: "Bp: The one-shot gold floor wave Niagara system exists, sized to the push radius."
parent: task-441-level-up-effect
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-441
- usage: included in the executor run of task-441
- runs: 2
- milestone: M2
- started: 2026-11-05T20:20:10Z
- waiting_for: none
- result: New NS_LevelUpWave: User.WaveRadius 300 (C++ writes PushRadius), CPU sim, Loop Once 0.5 s; Core 1 flat ring growing to 2 x radius over 0.45 s with ease-out; Sparks 24 flat gold shards, 0.4 to 0.5 s, running out radially. 25 sprites at most. M_LevelUpWave procedural gold (1, 0.72, 0.15) x 2. Compile UpToDate, 0 errors, 0 warnings. runs 2: the first build crashed the editor (GetUserVariables on a user variable added with the wrong field shape, nothing saved); rebuilt with name, type, description, defaultValue.

## Relations
- [[task-441-level-up-effect]] — parent global task.
