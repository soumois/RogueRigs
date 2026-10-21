---
id: task-280-01-drone-thruster-boost
description: "The drone thruster idle look equals its current top-speed look, and top speed is stronger than that."
parent: task-280-drone-thruster-boost
kind: task
status: done
---
# Drone thruster particles boosted

- goal: The drone thruster idle look equals its current top-speed look, and top speed is stronger than that.
- agent: bp
- depends_on: none
- inputs: bp-drone-thruster (vault/reference/tech/blueprints/fx/bp-drone-thruster.md), sys-drone-parts (vault/reference/tech/systems/combat/sys-drone/sys-drone-parts.md), drone (vault/reference/gdd/player/drone.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: NS_DroneThruster lerp endpoints (t = saturate(User.Speed / 700)), before -> after, idle = current top speed, top = stronger (proposed, provisional): Glow size lerp(7, 15) -> lerp(15, 22) cm, alpha lerp(0.2, 0.8) -> lerp(0.8, 1.0), colour (1, 1.2, 1.2) -> idle same, top x1.5 if a lerp is possible; Trail spawn lerp(3, 40) -> lerp(40, 70)/s, speed min lerp(20, 100) -> lerp(100, 150), max lerp(30, 125) -> lerp(125, 190), width lerp(2, 4.5) -> lerp(4.5, 6.5), length lerp(3, 20) -> lerp(20, 32), alpha lerp(0.25, 0.7) -> lerp(0.7, 1.0); bounds Fixed checked against the longer streaks. Compile, save, read back; bp-drone-thruster updated with before, after, revert values and the new particle count.
- done_when: Every lerp endpoint read back; system compiles (0 errors) and is saved; doc updated; lint clean.
- editor_state: open (editor + MCP)
- token_ceiling: 120k
- model: sonnet
- tokens: input 34, cache write 127724, cache read 1110257, output 612
- runs: 1
- milestone: M1
- result: Commit 5a1b8cfe. NS_DroneThruster lerp endpoints (idle, top), read back after save: Glow size (7, 15) -> (15, 22) cm, alpha (0.2, 0.8) -> (0.8, 1.0); Trail spawn (3, 40) -> (40, 70)/s, speed min (20, 100) -> (100, 150), max (30, 125) -> (125, 190), width (2, 4.5) -> (4.5, 6.5), length (3, 20) -> (20, 32), alpha (0.25, 0.7) -> (0.7, 1.0). Colours, lifetimes, cone, Trail bounds (-40,-40,-80)..(40,40,20) kept (longest streak about 70 cm). Compile UpToDate, 0 errors. Doc bp-drone-thruster (revert = old endpoints, about 20 live idle, 30 top). Not seen on screen.

## Relations
- [[task-280-drone-thruster-boost]] — parent global task.
