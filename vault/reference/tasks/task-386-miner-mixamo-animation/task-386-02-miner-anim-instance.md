---
id: task-386-02-miner-anim-instance
description: "Cpp: URRMinerAnimInstance: ground speed, direction to the aim, blend speed and play rate for ABP_Miner"
parent: task-386-miner-mixamo-animation
kind: task
status: done
---
# Miner animated with the owner Mixamo clips: cpp

- goal: Add the C++ anim instance URRMinerAnimInstance that ABP_Miner derives from: each update it reads the owning pawn velocity and actor yaw (which follows the aim) and exposes ground speed, direction relative to the aim, blend speed and play rate per Q4.
- agent: cpp
- depends_on: task-386-01-miner-clips-in-place
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), naming (vault/reference/tech/conventions/naming.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), sys-player (vault/reference/tech/systems/core/sys-player.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), vault/reference/tech/pipelines/miner-mixamo-clips.md (created by step 01), measured run speeds per direction: `Art/Source/miner-mixamo/miner_mixamo_clips.report.json`, owner words, planner measures and Q answers: global `request`, `gdd_check` and `## Questions` (vault/reference/tasks/task-386-miner-mixamo-animation.md), spec: `Source/RogueRigs/Player/RRMinerAnimInstance.h/.cpp`, `URRMinerAnimInstance : UAnimInstance` (not Abstract, so ABP_Miner can derive from it). `NativeUpdateAnimation`: from `TryGetPawnOwner()` velocity, `GroundSpeed` (cm/s, XY length), `Direction` (degrees -180 to 180, velocity relative to the actor yaw, 0 when still; `UKismetAnimationLibrary::CalculateDirection` or the same math), `RunSpeedForDirection` = linear interpolation between the four EditDefaultsOnly run speeds `RunSpeedForward`, `RunSpeedBackward`, `RunSpeedLeft`, `RunSpeedRight` (defaults = the RunFwd, RunBwd, RunLeft, RunRight speeds of the report), `LocomotionPlayRate` = clamp(GroundSpeed / RunSpeedForDirection, 1, `MaxPlayRate` 2.5) (provisional, owner Q4), `BlendSpeed` = GroundSpeed / LocomotionPlayRate. All BlueprintReadOnly, category `RogueRigs|Anim`; no gameplay logic, no input, no change to ARRMiner. Header comments state the units and the Q4 rule. Editor closed for the build.
- expected_output: URRMinerAnimInstance header and source, compiled; change summary (class, properties, defaults, commit hash) for the doc step.
- done_when: the editor target builds with no error and no new warning (Build.bat RogueRigsEditor Win64 Development); the class is not Abstract and exposes GroundSpeed, Direction, BlendSpeed, LocomotionPlayRate (BlueprintReadOnly) and the five EditDefaultsOnly tunables with the report values; committed by path as `task-386-02-miner-anim-instance: <summary>`.
- editor_state: closed
- token_ceiling: 0.6 M units
- model: opus
- tokens: input 22, cache write 42833, cache read 309351, output 8425
- usage: turns 11, context start 12k peak 43k, resumes 0, 3 min, units 0.13 M
- runs: 1
- milestone: M2
- started: 2026-10-30T05:37:44Z
- waiting_for: none
- result: URRMinerAnimInstance built (editor target OK, no new warning); GroundSpeed, Direction, BlendSpeed, LocomotionPlayRate + 5 tunables; commit bf05885d; Direction math inline (no AnimGraphRuntime dep); min play rate 1 fixed in code

## Relations
- [[task-386-miner-mixamo-animation]] — parent global task.
