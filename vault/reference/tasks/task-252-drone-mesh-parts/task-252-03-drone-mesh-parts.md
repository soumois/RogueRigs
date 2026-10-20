---
id: task-252-03-drone-mesh-parts
description: "Material and instance for the drone from the Meshy textures."
parent: task-252-drone-mesh-parts
kind: task
status: done
---
# Drone material

- goal: Material and instance for the drone from the Meshy textures.
- agent: bp
- depends_on: task-252-01-drone-mesh-parts
- inputs: drone (vault/reference/gdd/player/drone.md), sys-drone (vault/reference/tech/systems/combat/sys-drone.md), bp-drone (vault/reference/tech/blueprints/player/bp-drone.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), blueprints-player-index (vault/reference/tech/blueprints/player/blueprints-player-index.md), color-code (vault/reference/gdd/art-audio/color-code.md)
- expected_output: M_Drone and MI_Drone with the Meshy base colour, normal and metallic-roughness textures imported under /Game/RogueRigs/Art/Drone/, doc node.
- done_when: Material compiles, textures imported with right compression (normal map, linear for masks), MI_Drone exists, doc node written, lint clean.
- editor_state: open (editor + MCP)
- token_ceiling: 150k
- model: sonnet
- tokens: input 38, cache write 69497, cache read 873730, output 728
- runs: 1
- milestone: M1
- result: Materials under /Game/RogueRigs/Art/Drone/: T_Drone_BaseColor, T_Drone_Normal, T_Drone_MetalRough (4096 px, to cap), M_Drone and MI_Drone (base colour, normal, roughness G, metallic B), M_DroneAntenna and MI_DroneAntenna (vertex colour, Roughness 0.7); both compile; node bp-drone-materials. Look unverified, antenna vertex colours to check once the mesh is imported.

## Relations
- [[task-252-drone-mesh-parts]] — parent global task.
