---
id: task-468-01-unlimited-stop
description: "Cpp: Aim laser without length cap, ending on the first wall, robot or barrel, never over a wall, sharp tip."
parent: task-468-aim-laser-unlimited-length
kind: task
status: in-progress
---
# Aim laser without length limit, stopping at walls and robots: cpp

- goal: Aim laser without length cap, ending on the first wall, robot or barrel, never over a wall, sharp tip.
- agent: cpp
- depends_on: none
- inputs: aim-indicator (vault/reference/gdd/art-audio/aim-indicator.md), sys-aim-indicator (vault/reference/tech/systems/combat/sys-aim-indicator.md), miner (vault/reference/gdd/player/miner.md), task-423-miner-aim-laser-range-fade (vault/reference/tasks/task-423-miner-aim-laser-range-fade.md), Art/Source/aim-indicator/owner-capture-green-unlimited-2026-11-06.webp
- expected_output: see done_when
- done_when: Acceptance A1 to A5 hold in PIE or -game: no length cap; the line ends at border cliffs and breakable blocks without being drawn over them, on robots (shield included) and barrels, passes pickups, turrets, drone, console and extractor; tip ends sharp; width, glow, height, colour, visibility unchanged; no gameplay effect, aim assist untouched; tests broken by the removed cap or fade updated and passing; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-06T10:23:22Z
- waiting_for: none
- result: open

## Relations
- [[task-468-aim-laser-unlimited-length]] — parent global task.
