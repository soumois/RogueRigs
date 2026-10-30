---
id: task-386-17-simple-blendspace
description: "Bp: delete A_Miner_WalkBwd, rebuild BS_Miner_Locomotion with four samples on MoveX and MoveY, wire the Run state to it"
parent: task-386-miner-mixamo-animation
kind: task
status: done
---
# Miner animated with the owner Mixamo clips: simple 4-sample blend space

- goal: Delete A_Miner_WalkBwd with no reference left (old blend space, scripts, vault nodes); rebuild BS_Miner_Locomotion as a 2D blend space, axes X and Y from -1 to 1, four samples only; ABP_Miner Run state Blendspace Player fed MoveX and MoveY (Idle state and thresholds unchanged). If the asset names A_Miner_RunFwd/RunBwd/RunLeft/RunRight differ, map onto the matching clips and say so. Small groups, commit after each, MCP cannot edit state graphs: use the clipboard paste method of commits 8d5512c0 and 79c0c2ce.
- agent: bp
- depends_on: task-386-16-miner-move-vector
- inputs: Owner words 2026-10-30: simple blend space, direction as a vector (X forward/back, Y left/right), vector length = intensity (1 = max speed 600 uu/s); delete A_Miner_WalkBwd (bad clip); samples Back (-1,0) A_Miner_RunBwd, Fwd (1,0) A_Miner_RunFwd, Left (0,-1) A_Miner_RunLeft, Right (0,1) A_Miner_RunRight; idle stays in the Idle/Run state machine, thresholds unchanged; Tools/unreal/mcp-tool-sheet.md, bp-mcp-lessons, Tools/unreal/build_miner_blendspace.py, ABP_Miner and BS_Miner_Locomotion in /Game/RogueRigs/Characters/Miner
- expected_output: change summary, read-back of samples and pins, commit hashes
- done_when: A_Miner_WalkBwd deleted with references grep-clean; BS has 4 valid samples; ABP compiled, saved, committed by path as task-386-17-simple-blendspace: <summary>
- editor_state: open
- token_ceiling: 0.6 M units
- model: sonnet
- tokens: input 100, cache write 101929, cache read 2801784, output 20727
- usage: turns 50, context start 12k peak 102k, resumes 0, 4 min, units 0.51 M
- runs: 1
- milestone: M2
- started: 2026-10-30T16:43:24Z
- waiting_for: none
- result: A_Miner_WalkBwd deleted (no referencer, scripts fixed); BS_Miner_Locomotion = 4 samples on MoveX/MoveY (RunFwd, RunBwd, RunLeft, RunRight); ABP Run state rewired by clipboard paste; compiled clean; commits ce4723c1, ccc883ef; runtime unverified

## Relations
- [[task-386-miner-mixamo-animation]] — parent global task.