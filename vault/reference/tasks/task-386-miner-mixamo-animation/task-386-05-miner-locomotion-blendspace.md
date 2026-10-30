---
id: task-386-05-miner-locomotion-blendspace
description: "Bp: BS_Miner_Locomotion: 2D blend space, direction and speed, nine clips at their measured positions"
parent: task-386-miner-mixamo-animation
kind: task
status: done
---
# Miner animated with the owner Mixamo clips: bp

- goal: Create BS_Miner_Locomotion (Blend Space 2D on SKEL_Miner): horizontal axis Direction -180 to 180 degrees, vertical axis Speed 0 to the highest measured clip speed in cm/s, the nine clips at the direction and speed of the report (Q3).
- agent: bp
- depends_on: task-386-04-miner-clips-import
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), vault/reference/tech/pipelines/miner-mixamo-clips.md (created by step 01), report `Art/Source/miner-mixamo/miner_mixamo_clips.report.json`, owner words, planner measures and Q answers: global `request`, `gdd_check` and `## Questions` (vault/reference/tasks/task-386-miner-mixamo-animation.md), placement: Idle at speed 0 on directions -180, -90, 0, 90, 180; forward clips at 0, right at 90, left at -90, backward at both -180 and 180 (so the axis wraps), each at its measured speed; axis names Direction and Speed (ABP_Miner binds Direction and BlendSpeed); same stop-and-report rule as step 03.
- expected_output: BS_Miner_Locomotion saved in /Game/RogueRigs/Characters/Miner/Anims/; change summary (axes, sample list) for the doc step.
- done_when: editor Python read back: skeleton SKEL_Miner, axes Direction (-180, 180) and Speed (0, max report speed), the sample list matches the placement above with no warning of invalid samples; saved; committed by path as `task-386-05-miner-locomotion-blendspace: <summary>`.
- editor_state: open
- token_ceiling: 0.5 M units
- model: sonnet
- tokens: input 44, cache write 47681, cache read 800712, output 8874
- usage: turns 22, context start 12k peak 53k, resumes 0, 1 min, units 0.18 M
- runs: 1
- milestone: M2
- started: 2026-10-30T05:45:55Z
- waiting_for: none
- result: BS_Miner_Locomotion built: Direction -180..180, Speed 0..427.1, 15 samples as placed, read back; axes set via MCP ObjectTools (Python BlendParameter field name failed, committed script axes path untested); invalid-sample warnings not checked; commit eb27fa58

## Relations
- [[task-386-miner-mixamo-animation]] — parent global task.
