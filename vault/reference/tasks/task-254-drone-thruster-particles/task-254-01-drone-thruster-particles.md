---
id: task-254-01-drone-thruster-particles
description: "Two engine particles, one per side pod, driven by the drone speed."
parent: task-254-drone-thruster-particles
kind: task
status: in-progress
---
# Drone thruster particles

- goal: Two engine particles, one per side pod, driven by the drone speed.
- agent: bp
- depends_on: none
- inputs: drone (vault/reference/gdd/player/drone.md), sys-drone (vault/reference/tech/systems/combat/sys-drone.md), bp-drone (vault/reference/tech/blueprints/player/bp-drone.md), bp-dig-impact (vault/reference/tech/blueprints/fx/bp-dig-impact.md), blueprints-fx-index (vault/reference/tech/blueprints/fx/blueprints-fx-index.md), color-code (vault/reference/gdd/art-audio/color-code.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md)
- expected_output: NS_DroneThruster Niagara system (soft white with a faint teal tint, no red, yellow or cyan, faint idle glow, User.Speed driven size and rate) with its doc node.
- done_when: NS_DroneThruster exists, compiles, User parameter Speed documented in a bp-drone-thruster node; blueprints-fx-index updated; lint clean.
- editor_state: open (editor + MCP)
- token_ceiling: 150k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-254-drone-thruster-particles]] — parent global task.
