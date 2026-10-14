---
id: task-172-01-debris-start-jitter
description: "Debris pieces start at their place on the robot plus a random offset of up to 10 cm on each axis."
parent: task-172-debris-start-jitter
kind: task
status: todo
---
# Robot debris: random start position

- goal: Debris pieces start at their place on the robot plus a random offset of up to 10 cm on each axis.
- agent: bp
- depends_on: none
- inputs: enemy-death (vault/reference/gdd/enemies/enemy-death.md), bp-robot-debris (vault/reference/tech/blueprints/fx/bp-robot-debris.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), orchestrator-lessons-2026-10-14 (vault/reference/agents/orchestrator-lessons-2026-10-14.md)
- expected_output: NS_RobotDebris: at spawn each particle position gets a random offset uniform in [-10, 10] cm on X, Y and Z (Z kept above the floor plane: clamp so the piece does not start under the system Z), added after the baked rest offset; doc bp-robot-debris updated
- done_when: The system compiles with no stack issue and stays one-shot (finishes on its own); the changed module inputs read back; saved by explicit asset path; bp-robot-debris doc updated; committed by path; the editor stays open
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M3
- result: open

## Relations
- [[task-172-debris-start-jitter]] — parent global task.
