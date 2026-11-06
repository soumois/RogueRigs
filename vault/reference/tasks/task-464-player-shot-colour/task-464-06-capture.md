---
id: task-464-06-capture
description: "Capture: Before/after capture of a fight with rifle, turret, shotgun and enemy shots and flashes, Shooter present, sent to the owner"
parent: task-464-player-shot-colour
kind: task
status: todo
---
# Player shot colour: capture

- goal: Before/after capture of a fight with rifle, turret, shotgun and enemy shots and flashes, Shooter present, sent to the owner
- agent: capture
- depends_on: task-464-05-doc
- inputs: color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), Art/Source/muzzle-flash/owner-ref-green-laser-muzzle-2026-11-05.webp
- expected_output: see done_when
- done_when: Frames before and after in the same fight show: C1 rifle laser shots and impact sparks in varied green; C2 turret shots and impact sparks in the same green; C3 rifle and turret muzzle flashes green with several tones, compared with the reference, the turret flash at its barrel; C4 shotgun pellets and shotgun flash white-gold as before; C5 enemy shots and targeting lasers red; C6 the acid green Shooter stands out among the green shots; C7 mining and aim lasers white-gold; C8 green shots distinct from heal, XP and ore pickups. Frames sent to the owner.
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
