---
id: task-386-04-miner-clips-import
description: "Bp: import the nine in-place clips as AnimSequences on SKEL_Miner"
parent: task-386-miner-mixamo-animation
kind: task
status: todo
---
# Miner animated with the owner Mixamo clips: bp

- goal: Import the nine `Art/Meshes/Miner/Anims/A_Miner_*.fbx` as AnimSequences on SKEL_Miner in `/Game/RogueRigs/Characters/Miner/Anims/`, looping, root motion off.
- agent: bp
- depends_on: task-386-03-miner-skeletal-import
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), vault/reference/tech/pipelines/miner-mixamo-clips.md (created by step 01), clips: `Art/Meshes/Miner/Anims/A_Miner_*.fbx`; report `Art/Source/miner-mixamo/miner_mixamo_clips.report.json`, owner words, planner measures and Q answers: global `request`, `gdd_check` and `## Questions` (vault/reference/tasks/task-386-miner-mixamo-animation.md), method: editor Python `FbxImportUI` + `AssetImportTask` with `skeleton` = SKEL_Miner, `import_mesh` false, `import_animations` true; on each sequence root motion disabled, loop on; same stop-and-report rule as step 03.
- expected_output: nine AnimSequences A_Miner_Idle, RunFwd, WalkFwd, RunBwd, WalkBwd, RunLeft, StrafeLeft, RunRight, StrafeRight saved; change summary for the doc step.
- done_when: editor Python read back: nine AnimSequences on SKEL_Miner, play length within 0.05 s of the report, root motion disabled; no extra skeleton or mesh created by the import; committed by path as `task-386-04-miner-clips-import: <summary>`.
- editor_state: open
- token_ceiling: 0.5 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-386-miner-mixamo-animation]] — parent global task.
