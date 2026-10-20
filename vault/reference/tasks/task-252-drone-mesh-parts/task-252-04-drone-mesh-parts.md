---
id: task-252-04-drone-mesh-parts
description: "Import the three drone parts and assign the material."
parent: task-252-drone-mesh-parts
kind: task
status: in-progress
---
# Drone meshes import

- goal: Import the three drone parts and assign the material.
- agent: bp
- depends_on: task-252-02-drone-mesh-parts, task-252-03-drone-mesh-parts
- inputs: drone (vault/reference/gdd/player/drone.md), sys-drone (vault/reference/tech/systems/combat/sys-drone.md), bp-drone (vault/reference/tech/blueprints/player/bp-drone.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), blueprints-player-index (vault/reference/tech/blueprints/player/blueprints-player-index.md), color-code (vault/reference/gdd/art-audio/color-code.md)
- expected_output: SM_DroneBase, SM_DroneLaser, SM_DroneAntenna imported (centimetres, no collision, Nanite off, one LOD) with MI_Drone assigned, bounds and pivots read back, doc node.
- done_when: The three static meshes exist with MI_Drone, bounds in cm logged (base about 70 uu wide), pivots as the report says, doc node written, lint clean.
- editor_state: open (editor + MCP)
- token_ceiling: 150k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-252-drone-mesh-parts]] — parent global task.
