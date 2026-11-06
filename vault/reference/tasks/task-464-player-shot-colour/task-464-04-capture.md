---
id: task-464-04-capture
description: "Capture: Before/after capture of a fight with player and enemy shots, sent to the owner"
parent: task-464-player-shot-colour
kind: task
status: todo
---
# Player shot colour: capture

- goal: Before/after capture of a fight with player and enemy shots, sent to the owner
- agent: capture
- depends_on: task-464-03-doc
- inputs: color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md)
- expected_output: see done_when
- done_when: Frames before and after in the same fight show: C1 rifle, shotgun and turret shots and impact sparks in the new colour; C2 enemy shots and targeting lasers red; C3 player and enemy shots told apart at a glance; C4 player shots distinct from XP, heal and ore pickups; C5 mining and aim lasers white-gold. Frames sent to the owner.
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
- [[task-464-player-shot-colour]] — parent global task.
