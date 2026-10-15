---
id: task-190-01-debris-spin-damping
description: "Debris spin 0.3-0.8 rev/s, half-life 1 s in the air, halved on bounces, stopped at rest on the floor."
parent: task-190-debris-spin-damping
kind: task
status: in-progress
---
# Robot debris: slower, damped spin

- goal: Debris spin slower and damped: 0.3-0.8 rev/s per piece, half-life 1 s in the air, halved at each bounce, stopped once at rest on the floor.
- agent: bp
- depends_on: none
- inputs: bp-robot-debris (vault/reference/tech/blueprints/fx/bp-robot-debris.md), enemy-death (vault/reference/gdd/enemies/enemy-death.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md)
- expected_output: NS_RobotDebris: the fixed 1.5 rev/s UpdateMeshOrientation rotation replaced by a per-particle spin rate (Particles.SpinSpeed, random 0.3-0.8 rev/s at spawn, around the existing random Particles.SpinRate axis) that decays exponentially (multiply by 0.5^(DeltaTime/1.0) each update), is halved on each floor bounce (collision event or a velocity Z sign change near the floor plane, whichever works through MCP) and set to 0 when the piece rests on the floor (speed below about 20 cm/s near the plane); lifetime, sink, throw, jitter and one-shot settings unchanged; bp-robot-debris and the enemy-death GDD (spin line, provisional, owner 2026-10-15) updated
- done_when: The new modules and expressions read back; compiles with no stack issue; saved by explicit path; docs updated; committed by path; a -game dump (fx.Niagara.DumpComponents) shows the spin speed decreasing over age and 0 for pieces at rest (values, no screenshots)
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M3
- result: open

## Relations
- [[task-190-debris-spin-damping]] — parent global task.
