---
id: task-254-03-drone-thruster-particles
description: "Place the two thruster effects on the pods in BP_Drone."
parent: task-254-drone-thruster-particles
kind: task
status: done
---
# Drone thruster placement

- goal: Place the two thruster effects on the pods in BP_Drone.
- agent: bp
- depends_on: task-254-02-drone-thruster-particles, task-253-02-drone-parts-animation
- inputs: drone (vault/reference/gdd/player/drone.md), sys-drone (vault/reference/tech/systems/combat/sys-drone.md), bp-drone (vault/reference/tech/blueprints/player/bp-drone.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), blueprints-player-index (vault/reference/tech/blueprints/player/blueprints-player-index.md), color-code (vault/reference/gdd/art-audio/color-code.md)
- expected_output: BP_Drone thruster offsets set on the two side pods, doc updated.
- done_when: Offsets read back by MCP match the pods of SM_DroneBase, bp-drone updated, lint clean.
- editor_state: open (editor + MCP)
- token_ceiling: 150k
- model: sonnet
- tokens: input 36, cache write 34971, cache read 519348, output 288
- runs: 1
- milestone: M1
- result: Thruster offsets measured on SM_DroneBase (lowest pod ring centres (2.43, 27.67) and (2.38, -27.94) Blender, z -15.6) and set on BP_Drone: ThrusterLeftOffset (2.4, -27.7, -16.6), ThrusterRightOffset (2.4, 27.7, -16.6); compiled, saved. Look unverified.

## Relations
- [[task-254-drone-thruster-particles]] — parent global task.
