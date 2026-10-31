---
id: task-410-02-doc-rifle-offset
description: "Doc: sys-player-classes (cvar, live IK refresh through it), bp-burst-rifle and bp-miner (how to tune the offset and keep it)"
parent: task-410-rifle-offset-live-tuning
kind: task
status: todo
---
# Live control of the rifle offset in the right hand: doc

- goal: Document rr.Debug.RifleOffset from the cpp change summary: in sys-player-classes the cvar and the fact that the left hand follows a value set through it (the task-404 refresh limit stays for a Details panel edit); in bp-burst-rifle and bp-miner the tuning recipe: type the cvar in PIE, read the log, copy RifleMesh Location and Muzzle into BP_BurstRifle to keep them.
- agent: doc
- depends_on: task-410-01-rifle-offset-cvar
- inputs: task-410-rifle-offset-live-tuning (vault/reference/tasks/task-410-rifle-offset-live-tuning.md), agent-doc (vault/reference/agents/agent-doc.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), bp-burst-rifle (vault/reference/tech/blueprints/player/bp-burst-rifle.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md)
- expected_output: Updated vault/reference/tech/systems/core/sys-player/sys-player-classes.md, vault/reference/tech/blueprints/player/bp-burst-rifle.md, vault/reference/tech/blueprints/player/bp-miner.md; one commit `task-410-02-doc-rifle-offset: <summary>`.
- done_when: The three nodes name the cvar, its syntax and units, what it moves (rifle relative to the right hand, Muzzle by the same delta, left hand IK refreshed), the empty reset, that nothing is saved and how to keep a value in BP_BurstRifle; the task-404 refresh limit line says it still applies to a Details panel edit; unverified items stated (no PIE run by the crew); lint clean; committed by path.
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
- [[task-410-rifle-offset-live-tuning]] — parent global task.
