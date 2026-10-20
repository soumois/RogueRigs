---
id: task-254-01-drone-thruster-particles
description: "Two engine particles, one per side pod, driven by the drone speed."
parent: task-254-drone-thruster-particles
kind: task
status: done
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
- model: sonnet
- tokens: input 62, cache write 99529, cache read 2055303, output 728
- runs: 1
- milestone: M1
- result: NS_DroneThruster at /Game/RogueRigs/FX/Drone/ (+ M_DroneThruster): soft white teal-tinted glow and downward streaks, local space, CPU, at most about 20 particles; User.Speed (uu/s, 700 = top speed) drives size, count, length and alpha, idle glow about 25 percent. Compiles 0 errors; node bp-drone-thruster. Look and C++ drive unverified.

## Relations
- [[task-254-drone-thruster-particles]] — parent global task.
