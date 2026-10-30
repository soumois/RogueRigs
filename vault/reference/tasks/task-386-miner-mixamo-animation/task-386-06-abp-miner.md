---
id: task-386-06-abp-miner
description: "Bp: ABP_Miner: Animation Blueprint of URRMinerAnimInstance playing BS_Miner_Locomotion"
parent: task-386-miner-mixamo-animation
kind: task
status: done
---
# Miner animated with the owner Mixamo clips: bp

- goal: Create ABP_Miner (Animation Blueprint, parent class URRMinerAnimInstance, skeleton SKEL_Miner) whose AnimGraph plays BS_Miner_Locomotion with X = Direction, Y = BlendSpeed and play rate = LocomotionPlayRate into the output pose; no logic in its event graph.
- agent: bp
- depends_on: task-386-02-miner-anim-instance, task-386-05-miner-locomotion-blendspace
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), vault/reference/tech/pipelines/miner-mixamo-clips.md (created by step 01), owner words, planner measures and Q answers: global `request`, `gdd_check` and `## Questions` (vault/reference/tasks/task-386-miner-mixamo-animation.md), the C++ properties come from step 02 (`Source/RogueRigs/Player/RRMinerAnimInstance.h`); editor launched after the step 02 build. First Animation Blueprint built by the crew: if the AnimGraph cannot be built through MCP or editor Python, stop and report what failed, do not improvise another route.
- expected_output: ABP_Miner saved in /Game/RogueRigs/Characters/Miner/, compiled; change summary (parent, skeleton, graph nodes and bindings) for the doc step.
- done_when: ABP_Miner compiles with no error or warning; read back: parent class RRMinerAnimInstance, target skeleton SKEL_Miner, AnimGraph = one Blend Space Player on BS_Miner_Locomotion bound to Direction, BlendSpeed and LocomotionPlayRate, linked to Output Pose; event graph empty; saved; committed by path as `task-386-06-abp-miner: <summary>`.
- editor_state: open
- token_ceiling: 0.6 M units
- model: sonnet
- tokens: input 70, cache write 69956, cache read 1654499, output 15645
- usage: turns 35, context start 12k peak 75k, resumes 0, 5 min, units 0.33 M
- runs: 1
- milestone: M2
- started: 2026-10-30T05:47:35Z
- waiting_for: none
- result: ABP_Miner built and compiled (warnings_as_errors ok): parent RRMinerAnimInstance, skeleton SKEL_Miner, Blendspace Player BS_Miner_Locomotion bound to Direction, BlendSpeed, LocomotionPlayRate -> Output Pose; created via editor Python (BlueprintTools.create fails for AnimBlueprint); event graph only default unconnected event (emptiness not fully proven); commit 57edd0aa

## Relations
- [[task-386-miner-mixamo-animation]] — parent global task.
