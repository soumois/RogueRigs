---
id: task-369-03-bp-shuttle-extractor-look
description: "Bp: BP_ExtractionShuttle shows SM_Extractor_Body and the four rotors at their pivots, glow pulse removed, chevrons kept."
parent: task-369-extractor-level-exit
kind: task
status: todo
---
# Extractor in game: the level exit you walk up to: bp

- goal: BP_ExtractionShuttle looks like the extractor: body mesh, four rotors at their report pivots, the grey-box glow pulse removed from OnCountdown, the chevrons kept (Q2).
- agent: bp
- depends_on: task-369-02-cpp-shuttle-rotor-components
- inputs: bp-extraction-shuttle (vault/reference/tech/blueprints/world/bp-extraction-shuttle.md), bp-rr-game-mode (vault/reference/tech/blueprints/player/bp-rr-game-mode.md), agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), art-prompts-extractor (vault/reference/gdd/art-audio/art-prompts-extractor.md), Art/Meshes/extractor/extractor_rotors.report.json (rotor pivots)
- expected_output: BP_ExtractionShuttle.uasset updated (Mesh = SM_Extractor_Body at scale 1, origin on the footprint centre at floor height, Rotor* meshes and relative locations, OnCountdown without the GlowAmount call); one commit; change summary.
- done_when: PIE with SkipToExtraction: the shuttle spawns with the extractor body and four rotors, no white box, chevrons placed as before, walking to it gives the success screen, no log error; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-369-extractor-level-exit]] — parent global task.
