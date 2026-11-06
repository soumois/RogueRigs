---
id: task-464-03-capture
description: "Capture: Before/after capture of a fight with rifle, shotgun, turret and enemy shots, Shooter present, sent to the owner"
parent: task-464-player-shot-colour
kind: task
status: todo
---
# Player shot colour: capture

- goal: Before/after capture of a fight with rifle, shotgun, turret and enemy shots, Shooter present, sent to the owner
- agent: capture
- depends_on: task-464-02-doc
- inputs: color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md)
- expected_output: see done_when
- done_when: Frames before and after in the same fight show: C1 rifle laser shots and their impact sparks pure green; C2 shotgun pellets and turret bullets white-gold as before; C3 enemy shots and targeting lasers red; C4 the acid green Shooter stands out among the green shots; C5 muzzle flash, mining and aim lasers white-gold; C6 green shots distinct from heal, XP and ore pickups. Frames sent to the owner.
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
