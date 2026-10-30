---
id: task-386-23-run-clip-asset-check
description: "Bp: read the four run AnimSequences in Unreal and test the blend space with all samples set to one clip"
parent: task-386-miner-mixamo-animation
kind: task
status: done
---
# Miner animated with the owner Mixamo clips: run clip assets in Unreal

- goal: Owner hypothesis: the defect comes from the RunBwd, RunLeft or RunRight AnimSequence in Unreal (arm tracks, skeleton, compression), not from the blend space. Read the four AnimSequences (skeleton, length, bone track count, arm bones, compression settings, additive type) and compare with RunFwd; then set all four samples of BS_Miner_Locomotion to A_Miner_RunFwd (test A) so the capture can show whether arms hold.
- agent: bp
- depends_on: task-386-22-capture-four-samples
- inputs: task-386-22 capture, BS_Miner_Locomotion and ABP_Miner in /Game/RogueRigs/Characters/Miner, Tools/unreal/check_miner_anim.py, bp-mcp-lessons
- expected_output: report
- done_when: read-back table per clip; BS samples all A_Miner_RunFwd, saved; not committed (temporary test) unless a clip defect is found and fixed
- editor_state: open
- token_ceiling: 0.4 M units
- model: sonnet
- tokens: input 16, cache write 26834, cache read 143762, output 5784
- usage: turns 8, context start 12k peak 27k, resumes 0, 1 min, units 0.08 M
- runs: 1
- milestone: M2
- started: 2026-10-30T17:09:50Z
- waiting_for: none
- result: 4 AnimSequences identical in skeleton/compression/additive/root settings, only lengths differ; per-bone tracks unreadable via MCP; test A applied (all 4 samples = RunFwd, uncommitted)

## Relations
- [[task-386-miner-mixamo-animation]] - parent global task.
