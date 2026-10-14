---
id: task-174-02-debris-position-fix
description: "Fix the NS_RobotDebris spawn position (vector max collapsed to one scalar) and check in a game run that pieces start at the robot."
parent: task-174-debris-regression
kind: task
status: in-progress
---
# Robot debris: spawn position fix

- goal: Fix the NS_RobotDebris spawn position: remove the vector max from the Position entry and clamp Z in a separate module, then check in a game run that pieces start at the robot.
- agent: bp
- depends_on: task-174-01-debris-regression
- inputs: bp-robot-debris (vault/reference/tech/blueprints/fx/bp-robot-debris.md), enemy-death (vault/reference/gdd/enemies/enemy-death.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md)
- expected_output: Position entry = Particles.Position + rotated PieceLocal + jitter (no max); a new SetParameters module right after it (before SpinRate): Particles.Position = float3(Particles.Position.x, Particles.Position.y, max(Particles.Position.z, Engine.Owner.Position.z)); bp-robot-debris updated, with the quirk (vector max in an HLSL SetParameters input compiles to one scalar) added to unreal-mcp-quirks
- done_when: The system compiles with no stack issue; expressions read back; a -game dump (fx.Niagara.DumpComponents) shows spawn Previous.Position near the dead robot with three different components; saved by explicit path; docs updated; committed by path
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M3
- result: open

## Relations
- [[task-174-debris-regression]] — parent global task.
