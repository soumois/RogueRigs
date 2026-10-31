---
id: task-408-05-doc-upper-rifle-variants
description: "Doc: doc: bp-miner (variants, how to pick one as UpperBodySequence) and sys-player-classes (MinerAnimFreeze, MinerUpperPose)"
parent: task-408-miner-right-arm-in-animation
kind: task
status: todo
---
# Miner right arm corrected in the animation, variants compared on one frame: doc

- goal: bp-miner and sys-player-classes describe the variant sequences, how to choose one and the two debug cvars; nothing of the removed task-407 offsets remains.
- agent: doc
- depends_on: task-408-01-remove-arm-offsets-upper-pose-cvar, task-408-03-import-upper-rifle-variants
- inputs: bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), agent-doc (vault/reference/agents/agent-doc.md)
- expected_output: Nodes updated, values left open, lint result, commit hash.
- done_when: bp-miner lists A_Miner_UpperRifle_V1..V4 with their (E, H) values, the procedure to pick one (UpperBodySequence in ABP_Miner Class Defaults) and the capture sheet paths; sys-player-classes lists rr.Debug.MinerAnimFreeze and rr.Debug.MinerUpperPose and no right arm offsets; unverified items stated; lint clean; committed by path.
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
- [[task-408-miner-right-arm-in-animation]] — parent global task.
