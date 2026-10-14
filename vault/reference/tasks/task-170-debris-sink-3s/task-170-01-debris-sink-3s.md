---
id: task-170-01-debris-sink-3s
description: "Debris pieces stay 3 s at full size, then sink into the floor in 0.4 s without shrinking (3.4 s in all)."
parent: task-170-debris-sink-3s
kind: task
status: in-progress
---
# Robot debris: 3 s, then sink in 0.4 s

- goal: Debris pieces stay 3 s at full size, then sink into the floor in 0.4 s without shrinking (3.4 s in all).
- agent: bp
- depends_on: none
- inputs: enemy-death (vault/reference/gdd/enemies/enemy-death.md), bp-robot-debris (vault/reference/tech/blueprints/fx/bp-robot-debris.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), orchestrator-lessons-2026-10-14 (vault/reference/agents/orchestrator-lessons-2026-10-14.md)
- expected_output: NS_RobotDebris: particle lifetime 3.4 s; the end-of-life shrink removed (scale stays 1); from age 3.0 s to 3.4 s the piece moves down into the floor (about its own height, e.g. 60 cm, so it is fully under the floor plane) and the floor collision no longer holds it during that phase (or the collision plane is lowered with it); system loop settings still one-shot; doc bp-robot-debris updated
- done_when: The system compiles with no stack issue and stays one-shot (finishes on its own); the changed module inputs read back; saved by explicit asset path; bp-robot-debris doc updated; committed by path; the editor stays open
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M3
- result: open

## Relations
- [[task-170-debris-sink-3s]] — parent global task.
