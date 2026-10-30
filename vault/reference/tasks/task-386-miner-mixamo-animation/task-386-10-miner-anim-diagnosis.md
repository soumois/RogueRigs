---
id: task-386-10-miner-anim-diagnosis
description: "Bp: diagnose why the miner stays in T-pose in game, step by step without guessing, and fix the cause found"
parent: task-386-miner-mixamo-animation
kind: task
status: in-progress
---
# Miner animated with the owner Mixamo clips: bp diagnosis

- goal: Find, with read-back evidence and no guessing, why the miner stays in T-pose in a -game run (owner 2026-10-30: "Il n'y a strictement aucune animation qui se joue, le personnage reste en T pose"), and fix the cause found.
- agent: bp
- depends_on: task-386-07-bp-miner-mesh-swap
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), vault/reference/tech/pipelines/miner-mixamo-clips.md, capture frames `Saved/Captures/MinerAnim2/` (idle_00.png, crops-idle.png), report of steps 03 to 07 (commits b35afcb7, 31bfbde6, eb27fa58, 57edd0aa, 46f17540), scripts `Tools/unreal/` (import_miner_skeletal.py, import_miner_clips.py, build_miner_blendspace.py), global `request` and `## Questions` (vault/reference/tasks/task-386-miner-mixamo-animation.md). Order of checks, simplest first, each with its read-back recorded: (1) the nine AnimSequences really animate SKEL_Miner: track counts, target bones, not empty, a bone value changes over time (open a preview if the editor allows, or read curves by Python); (2) BP_Miner Mesh AnimationMode = AnimationBlueprint and AnimClass = ABP_Miner_C, on the CDO and on the BP_Miner instance placed in the map; (3) ABP_Miner: Blendspace Player linked to Output Pose, blend space assigned, Direction, BlendSpeed, LocomotionPlayRate pins bound to existing variables, no hidden compile error, saved with its graph (the committed .uasset holds the graph); (4) BS_Miner_Locomotion: the 15 samples are valid (no invalid sample), preview at speed 0 gives the Idle pose; (5) at runtime, log Speed and Direction of the ABP instance in a -game or PIE run to show it runs. Also check by `git status` that every asset created by Python is saved on disk and committed (the editor closed without being seen to). Cause found: fix it, record what was wrong. If a step needs a manual gesture of the owner in the editor, stop and say precisely which.
- expected_output: a diagnosis line per check (ok or defect, evidence), the fix and its commit, change summary for the doc step.
- done_when: each of the five checks has a recorded result; the cause is named with its evidence; the fix is read back (for example a PIE log showing the ABP running with a non-idle pose); modified assets saved and committed by path as `task-386-10-miner-anim-diagnosis: <summary>`; or, when no cause is found or a manual step is needed, a precise report to the Executor.
- editor_state: open
- token_ceiling: 0.6 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-30T06:09:35Z
- waiting_for: none
- result: open

## Relations
- [[task-386-miner-mixamo-animation]] — parent global task.
