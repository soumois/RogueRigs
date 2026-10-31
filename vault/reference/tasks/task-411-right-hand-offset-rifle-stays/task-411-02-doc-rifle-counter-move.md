---
id: task-411-02-doc-rifle-counter-move
description: "Doc: sys-player-classes, bp-miner, bp-burst-rifle: the offset moves the hand on a still rifle (counter-move), left hand refresh"
parent: task-411-right-hand-offset-rifle-stays
kind: task
status: todo
---
# Right hand offset moves the hand on the rifle, the rifle stays in place: doc

- goal: Document the task-411 counter-move from the cpp change summary: sys-player-classes (RefreshRightHandOffset counter-moves RifleMesh and Muzzle by O^-1 and refreshes the left hand IK; the rifle still follows the hand in animations); bp-miner (tuning recipe text: the hand slides on a still rifle; reach risk); bp-burst-rifle (RifleMesh and Muzzle are overridden at runtime by the counter-move, asset values stay (0, 0, 0) and (52.25, 0, 0)); correct the task-410 lines that say the rifle follows the offset.
- agent: doc
- depends_on: task-411-01-rifle-counter-move
- inputs: task-411-right-hand-offset-rifle-stays (vault/reference/tasks/task-411-right-hand-offset-rifle-stays.md), agent-doc (vault/reference/agents/agent-doc.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), bp-burst-rifle (vault/reference/tech/blueprints/player/bp-burst-rifle.md)
- expected_output: Updated vault/reference/tech/systems/core/sys-player/sys-player-classes.md, vault/reference/tech/blueprints/player/bp-miner.md, vault/reference/tech/blueprints/player/bp-burst-rifle.md; one commit `task-411-02-doc-rifle-counter-move: <summary>`.
- done_when: The three nodes state the counter-move rule, the left hand refresh, that the rifle follows the hand in animations but not the offset, the reach risk; no stale task-410 sentence says the offset moves the rifle; unverified items stated (no PIE run by the crew); lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-411-right-hand-offset-rifle-stays]] — parent global task.
