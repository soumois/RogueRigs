---
id: task-386-13-abp-palier2-run
description: "Bp: add Rifle Run to ABP_Miner with an idle to run state driven by ground speed from URRMinerAnimInstance"
parent: task-386-miner-mixamo-animation
kind: task
status: in-progress
---
# Miner animated with the owner Mixamo clips: step 2 run forward

- goal: ABP_Miner gets two states, Idle (Rifle Aiming Idle) and Run (Rifle Run, forward), with a transition on the ground speed given by URRMinerAnimInstance (run when speed above a small threshold, idle otherwise); no blend space yet, no direction. Verify the ABP instance reads a non-zero speed while the miner moves.
- agent: bp
- depends_on: task-386-12-capture-palier1
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), Tools/unreal/mcp-tool-sheet.md, bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), vault/reference/tech/pipelines/miner-mixamo-clips.md, scripts Tools/unreal/ (import_miner_*.py, build_miner_blendspace.py, probe scripts of commit 5aec1b80), global request (vault/reference/tasks/task-386-miner-mixamo-animation.md). Owner 2026-10-30: build by steps, see each work before adding the next; nothing works now (T-pose)
- expected_output: change summary; read-back of the state machine, transition rules, compile result, runtime log of Speed in PIE or -game; commit hash
- done_when: ABP_Miner state machine Idle and Run compiled without error, saved and committed by path as task-386-13-abp-palier2-run: <summary> with its .uasset; speed read back at runtime; editor commands hanging over 3 minutes are killed and retried
- editor_state: open
- token_ceiling: 0.5 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-30T06:30:11Z
- waiting_for: none
- result: open

## Relations
- [[task-386-miner-mixamo-animation]] — parent global task.
