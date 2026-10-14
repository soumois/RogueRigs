---
id: task-152-02-chaser-blueprint-swap
description: "Replace the grey-box body of BP_Chaser (and the shield chaser) with SM_Chaser and its four materials."
parent: task-152-chaser-static-integration
kind: task
status: in-progress
---
# Chaser: Blueprint swap

- goal: Replace the grey-box body of BP_Chaser (and the shield chaser) with SM_Chaser and its four materials.
- agent: bp
- depends_on: task-152-01-chaser-static-integration
- inputs: bp-chaser (vault/reference/tech/blueprints/enemies/bp-chaser.md), bp-shield-chaser (vault/reference/tech/blueprints/enemies/bp-shield-chaser.md), sys-enemies-chaser (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-chaser.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: BP_Chaser BodyMesh uses SM_Chaser (relative location 0, 0, -62 so the feet are at the capsule bottom: the capsule half height is 62 and the mesh origin is at the floor, relative scale 1, forward +X) with the four material overrides; the EyeMesh sphere hidden (the eye is in the model); the shield variant checked and adapted the same way; the telegraph blink, NoCollision and light-parts behaviour unchanged; doc bp-chaser and bp-shield-chaser updated
- done_when: both Blueprints compile and save; values read back; the doc nodes updated; no C++ change; the editor is left open; the owner is told how to see it (open L_ArenaM1 and play, rr.Stress.Spawn 10 to get a crowd)
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-152-chaser-static-integration]] — parent global task.
