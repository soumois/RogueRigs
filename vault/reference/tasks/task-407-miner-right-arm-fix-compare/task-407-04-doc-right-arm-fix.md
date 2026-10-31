---
id: task-407-04-doc-right-arm-fix
description: "Doc: doc: bp-miner (right arm correction, how to tune) and sys-player-classes (offsets and debug cvars)"
parent: task-407-miner-right-arm-fix-compare
kind: task
status: cancelled
---
# Miner right arm correction on the idle pose, compared on one frame: doc

- goal: bp-miner and sys-player-classes describe the right arm correction, the two offsets, how to tune them and the two debug cvars.
- agent: doc
- depends_on: task-407-01-right-arm-offsets-cvars, task-407-02-abp-right-arm-fix
- inputs: bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), agent-doc (vault/reference/agents/agent-doc.md)
- expected_output: Nodes updated, values left open, lint result, commit hash.
- done_when: bp-miner AnimGraph shows the right arm correction (method, nodes, spaces) and how to tune RightElbowOffset and RightHandOffset in ABP_Miner Class Defaults; sys-player-classes lists the two offsets, rr.Debug.MinerArmFix and rr.Debug.MinerAnimFreeze; the capture sheet paths and unverified items are stated; lint clean; committed by path.
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
- [[task-407-miner-right-arm-fix-compare]] — parent global task.
