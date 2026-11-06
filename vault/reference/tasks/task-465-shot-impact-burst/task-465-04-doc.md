---
id: task-465-04-doc
description: "Doc: Update the bp-* and sys-* nodes of the impact effects and check color-code, burst-rifle, shotgun and turret match the result"
parent: task-465-shot-impact-burst
kind: task
status: todo
---
# Player shot impact burst: doc

- goal: Update the bp-* and sys-* nodes of the impact effects and check color-code, burst-rifle, shotgun and turret match the result
- agent: doc
- depends_on: task-465-03-impact-expand
- inputs: color-code (vault/reference/gdd/art-audio/color-code.md), shotgun (vault/reference/gdd/weapons/shotgun.md), sys-weapons (vault/reference/tech/systems/combat/sys-weapons.md), task-432-player-shot-impact-sparks (vault/reference/tasks/task-432-player-shot-impact-sparks.md)
- expected_output: see done_when
- done_when: The bp-* and sys-* nodes describing the player shot impact effects state the new burst and its two colours; color-code, burst-rifle, shotgun and turret describe the impact look as built; lint clean.
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
- [[task-465-shot-impact-burst]] — parent global task.
