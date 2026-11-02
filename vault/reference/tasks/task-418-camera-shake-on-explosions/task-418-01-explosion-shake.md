---
id: task-418-01-explosion-shake
description: "Cpp: explosion shake preset played at each resolved explosion with distance falloff"
parent: task-418-camera-shake-on-explosions
kind: task
status: in-progress
---
# Camera shake from nearby explosions: cpp

- goal: Add an explosion shake preset and play it from URRExplosionSubsystem at each resolved explosion centre through the task-416 located call, strength and max distance per source.
- agent: cpp
- depends_on: none
- inputs: task-418-camera-shake-on-explosions (vault/reference/tasks/task-418-camera-shake-on-explosions.md), task-416-camera-shake-mixing-falloff (vault/reference/tasks/task-416-camera-shake-mixing-falloff.md), sys-camera-shake (vault/reference/tech/systems/core/sys-camera-shake.md), sys-explosions (vault/reference/tech/systems/combat/sys-explosions.md), explosions (vault/reference/gdd/terrain/explosions.md), barrel (vault/reference/gdd/terrain/barrel.md), enemy-kamikaze (vault/reference/gdd/enemies/enemy-kamikaze.md), agent-cpp (vault/reference/agents/agent-cpp.md)
- expected_output: C++: explosion preset class (0.35 s, 25 Hz), per-source strength and max-distance factor (ExplosionStats.csv columns or subsystem config, cpp choice, documented), the call in the resolution step; values provisional owner 2026-11-02; commit task-418-01-explosion-shake; change summary for the doc subtask. Uncommitted edits of task-415 (RRAimIndicatorComponent, three test files, Config/DefaultEditor.ini) are not this task's: do not touch or commit them.
- done_when: Editor build green; each resolved explosion calls the located shake once at its centre with barrel 10 uu / 1400 uu, kamikaze 7 uu / 624 uu; no call on fuse start; -nullrhi tests unaffected (no local camera = nothing played); full RogueRigs automation suite with no new failure; committed by path.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: 2026-11-02T18:35:47Z
- waiting_for: none
- result: open

## Relations
- [[task-418-camera-shake-on-explosions]] — parent global task.
