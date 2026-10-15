---
id: task-186-01-debris-throw-speed-range
description: "Debris throw: final speed 325-600 cm/s along the hit direction and 350-550 cm/s upward, per-death factor 1.3-2 kept, angle and spread unchanged."
parent: task-186-debris-throw-speed-range
kind: task
status: todo
---
# Robot debris: throw speed ranges

- goal: Debris throw: final speed 325-600 cm/s along the hit direction and 350-550 cm/s upward, per-death factor 1.3-2 kept, angle and spread unchanged.
- agent: bp
- depends_on: none
- inputs: bp-robot-debris (vault/reference/tech/blueprints/fx/bp-robot-debris.md), enemy-death (vault/reference/gdd/enemies/enemy-death.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: NS_RobotDebris particle spawn Velocity expression: the per-piece base ranges changed so that, with Emitter.ThrowScale F in [1.3, 2] unchanged, the final speed is 325-600 along the rotated hit direction (base 250-300) and 350-550 upward (base 269.2-275); the ±15 degree angle and the ±100 x F lateral spread unchanged; enemy-death throw line and bp-robot-debris updated with the final ranges
- done_when: The Velocity expression reads back with the new base ranges; compiles with no stack issue; saved by explicit path; docs updated; committed by path; the editor stays open
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M3
- result: open

## Relations
- [[task-186-debris-throw-speed-range]] — parent global task.
