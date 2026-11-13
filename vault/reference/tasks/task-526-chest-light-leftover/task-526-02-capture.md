---
id: task-526-02-capture
description: "Capture: next level after an opened chest"
parent: task-526-chest-light-leftover
kind: task
status: done
---
# Lumière du coffre restée du niveau précédent: capture

- goal: next level after an opened chest
- agent: capture
- depends_on: task-526-01-cleanup-fix
- inputs: agent-capture (vault/reference/agents/agent-capture.md)
- expected_output: see done_when
- done_when: C1 level N+1 at the former chest spot: no chest light; C2 an opened chest in its own level still glows; frames sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-526
- usage: included in the executor run of task-526
- runs: 2
- milestone: M2
- started: 2026-11-13T14:19:09Z
- waiting_for: none
- result: C1 seen: level 2 top-down frame (Saved/Captures/task526_chest2/c_00.png, camera pitch -89 arm 6000) shows no chest light at the former chest spot (level 1 chest at X 8576 Y 10117, level 2 miner at 8100, 8100: about 270 px right and 60 px up of the miner, purple rock, no glow) nor elsewhere; the red pool under the miner is its own light, also seen in level 1. Close frame d_00.png same. C2 seen: a_00.png, level 1, opened chest lid up with its yellow glow and rays. Log: OnLidOpened tracked 1, level change removed 3 [NS_BonusChestOpen, NS_HeavySlamImpact, InnerLight], level 2 start alive 0. 2 launches (first one had the level-up cards over the chest frames). Unverified: the deferred Niagara activation of the owner session did not occur in -game (system already compiled), so the frame shows the cleared case, not the reproduced defect.

## Relations
- [[task-526-chest-light-leftover]] — parent global task.
