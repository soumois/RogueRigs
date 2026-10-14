---
id: task-171-01-debris-throw-random
description: "Debris throw speed times a random 1.3-2 factor per death, each piece turned within ±15° of the killing-hit direction."
parent: task-171-debris-throw-random
kind: task
status: todo
---
# Robot debris: random force and angle

- goal: Debris throw: speed times a random factor 1.3 to 2 drawn once per death, and each piece turned by a random angle within plus or minus 15 degrees around the killing-hit direction.
- agent: bp
- depends_on: none
- inputs: enemy-death (vault/reference/gdd/enemies/enemy-death.md), bp-robot-debris (vault/reference/tech/blueprints/fx/bp-robot-debris.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), orchestrator-lessons-2026-10-14 (vault/reference/agents/orchestrator-lessons-2026-10-14.md)
- expected_output: NS_RobotDebris: one random factor in [1.3, 2] drawn once per system activation (emitter/system level, or a per-activation seed shared by the particles) multiplies the whole throw velocity (horizontal along the direction and upward), the existing per-piece spread kept; each particle rotates User.HitDirection around Z by its own random angle in [-15, +15] degrees before use; doc bp-robot-debris updated. If a per-activation random cannot be built through MCP, report it (a C++ user parameter User.ThrowScale would then be added by a cpp task), do not fake it per particle
- done_when: The system compiles with no stack issue and stays one-shot (finishes on its own); the changed module inputs read back; saved by explicit asset path; bp-robot-debris doc updated; committed by path; the editor stays open
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M3
- result: open

## Relations
- [[task-171-debris-throw-random]] — parent global task.
