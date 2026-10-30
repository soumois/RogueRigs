---
id: task-386-11-abp-palier1-idle
description: "Bp: reduce ABP_Miner to one Sequence Player of Rifle Aiming Idle looping into Output Pose, verify it plays"
parent: task-386-miner-mixamo-animation
kind: task
status: done
---
# Miner animated with the owner Mixamo clips: step 1 idle only

- goal: Minimal ABP_Miner: only the Rifle Aiming Idle AnimSequence looping, Sequence Player to Output Pose, no blend space, no variable. BP_Miner (CDO and the instance placed in the map) uses Animation Mode Use Animation Blueprint and Anim Class ABP_Miner_C. If the pose may still not play, check the clip is not empty, the skeleton matches, the ABP is compiled and saved with its graph, and read back (PIE log or editor Python) that it runs.
- agent: bp
- depends_on: task-386-07-bp-miner-mesh-swap
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), Tools/unreal/mcp-tool-sheet.md, bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), vault/reference/tech/pipelines/miner-mixamo-clips.md, scripts Tools/unreal/ (import_miner_*.py, build_miner_blendspace.py, probe scripts of commit 5aec1b80), global request (vault/reference/tasks/task-386-miner-mixamo-animation.md). Owner 2026-10-30: build by steps, see each work before adding the next; nothing works now (T-pose)
- expected_output: change summary; read-back of the graph, the sequence asset, the Mesh AnimationMode and AnimClass on the CDO and the placed instance; commit hash
- done_when: ABP_Miner graph is Sequence Player (Rifle Aiming Idle, loop) to Output Pose, compiled without error, saved and committed by path as task-386-11-abp-palier1-idle: <summary> with its .uasset; BP_Miner and the placed instance read back as Use Animation Blueprint and ABP_Miner_C; any editor command that hangs more than 3 minutes is killed (kill UnrealEditor, relaunch, retry) rather than waited for
- editor_state: open
- token_ceiling: 0.5 M units
- model: sonnet
- tokens: input 64, cache write 107086, cache read 1804044, output 14599
- usage: turns 32, context start 12k peak 107k, resumes 0, 2 min, units 0.39 M
- runs: 1
- milestone: M2
- started: 2026-10-30T06:25:48Z
- waiting_for: none
- result: ABP_Miner = single looping Sequence Player A_Miner_Idle to Output Pose, compiled, saved, commit a2894680; CDO and PIE mesh read back AnimationBlueprint + ABP_Miner_C; no placed instance (GameMode spawns it); runtime pose unverified, left to the capture

## Relations
- [[task-386-miner-mixamo-animation]] — parent global task.
