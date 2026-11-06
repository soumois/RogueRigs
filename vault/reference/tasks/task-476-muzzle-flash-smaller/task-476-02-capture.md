---
id: task-476-02-capture
description: "Capture: Before/after capture of the rifle flash, with a turret firing."
parent: task-476-muzzle-flash-smaller
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-476
- usage: included in the executor run of task-476
- runs: 1
- milestone: M2
- started: 2026-11-06T19:33:25Z
- waiting_for: none
- result: Done, 1 before and 2 after launches (first after run invalid: BP_Turret was not yet saved, see 01), same slomo 0.2 fight, aim yaw 0, same camera: Saved/Captures/task476_before, task476_after. Sheets for the owner: Saved/Captures/task476_compare_crop.png (rifle, turret, shotgun before/after), task476_compare_turret2.png (second turret frame). C1 seen: the rifle star at the miner is clearly smaller (about 140 against 200 px across in the crops). C2 seen: same pale core, bright green blades, seven-spike star and forward streak. C3 seen: turret star at the barrel the same size before and after. C4 seen: shotgun white-gold flash the same before and after. Nothing ambiguous.

## Relations
- [[task-476-muzzle-flash-smaller]] — parent global task.
