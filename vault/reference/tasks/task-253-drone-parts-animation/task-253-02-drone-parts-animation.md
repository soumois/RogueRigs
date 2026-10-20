---
id: task-253-02-drone-parts-animation
description: "Assign the three meshes to BP_Drone and remove the grey sphere."
parent: task-253-drone-parts-animation
kind: task
status: todo
---
# BP_Drone parts

- goal: Assign the three meshes to BP_Drone and remove the grey sphere.
- agent: bp
- depends_on: task-253-01-drone-parts-animation, task-252-04-drone-mesh-parts
- inputs: drone (vault/reference/gdd/player/drone.md), sys-drone (vault/reference/tech/systems/combat/sys-drone.md), bp-drone (vault/reference/tech/blueprints/player/bp-drone.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), blueprints-player-index (vault/reference/tech/blueprints/player/blueprints-player-index.md), color-code (vault/reference/gdd/art-audio/color-code.md)
- expected_output: BP_Drone: BaseMesh, LaserPivot and AntennaMesh set to the SM_Drone parts, grey-box sphere and MI_GB_Drone removed from the visible mesh, offsets set, bp-drone doc updated.
- done_when: BP_Drone compiles, three parts visible with the right offsets (read back by MCP), bp-drone updated, lint clean.
- editor_state: open (editor + MCP)
- token_ceiling: 150k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-253-drone-parts-animation]] — parent global task.
