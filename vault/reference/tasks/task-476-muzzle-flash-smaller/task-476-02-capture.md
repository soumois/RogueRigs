---
id: task-476-02-capture
description: "Capture: Before/after capture of the rifle flash, with a turret firing."
parent: task-476-muzzle-flash-smaller
kind: task
status: todo
---
# Player muzzle flash a little smaller: capture

- goal: Before/after capture of the rifle flash, with a turret firing.
- agent: capture
- depends_on: task-476-01-rifle-flash-size
- inputs: agent-capture (vault/reference/agents/agent-capture.md), Art/Source/muzzle-flash/owner-ref-green-laser-muzzle-2026-11-05.webp
- expected_output: see done_when
- done_when: C1 rifle flash visibly smaller than before at the same zoom; C2 same green tones, spikes and streak; C3 turret flash same size as before; C4 shotgun flash unchanged if a shotgun is available, else listed unseen; side-by-side sheet sent to the owner.
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
- [[task-476-muzzle-flash-smaller]] — parent global task.
