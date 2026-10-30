---
id: task-386-15-abp-palier3-directions
description: "Bp: add the other clips and directions to ABP_Miner, by steps, reusing or rebuilding BS_Miner_Locomotion"
parent: task-386-miner-mixamo-animation
kind: task
status: in-progress
---
# Miner animated with the owner Mixamo clips: step 3 other directions

- goal: Extend ABP_Miner to the walk and the directions (strafe left, right, backward, walk clips), reusing BS_Miner_Locomotion if its samples read back valid, else rebuilding it by small groups; add a group at a time, compile and save after each. Direction and speed come from URRMinerAnimInstance. Q3 and Q4 defaults of the global apply.
- agent: bp
- depends_on: task-386-14-capture-palier2
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), Tools/unreal/mcp-tool-sheet.md, bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), vault/reference/tech/pipelines/miner-mixamo-clips.md, scripts Tools/unreal/ (import_miner_*.py, build_miner_blendspace.py, probe scripts of commit 5aec1b80), global request (vault/reference/tasks/task-386-miner-mixamo-animation.md). Owner 2026-10-30: build by steps, see each work before adding the next; nothing works now (T-pose)
- expected_output: change summary; read-back per group; commit hash per group
- done_when: ABP_Miner plays the walk and the four directions through the blend space or states, compiled without error, saved and committed by path as task-386-15-abp-palier3-directions: <summary> with its .uasset; runtime log of Direction and Speed; hanging editor commands killed and retried
- editor_state: open
- token_ceiling: 0.8 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-30T06:45:26Z
- waiting_for: none
- result: open

## Relations
- [[task-386-miner-mixamo-animation]] — parent global task.
