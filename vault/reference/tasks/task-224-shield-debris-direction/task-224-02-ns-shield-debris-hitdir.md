---
id: task-224-02-ns-shield-debris-hitdir
description: "bp: NS_ShieldDebris gets User.HitDirection back and throws its pieces along it with the NS_RobotDebris velocity setup."
parent: task-224-shield-debris-direction
kind: task
status: todo
---
# NS_ShieldDebris: throw along User.HitDirection

- goal: the shield pieces fly like the robot pieces, toward User.HitDirection.
- agent: agent-bp
- depends_on: task-224-01-shield-debris-direction-cpp
- inputs: `vault/reference/tech/blueprints/fx/bp-shield-debris.md` (current velocity: owner forward +/-15 degrees, no user parameter), `vault/reference/tech/blueprints/fx/bp-robot-debris.md` (User.HitDirection and its velocity expression, tools that worked and failed), `vault/reference/agents/bp-mcp-lessons.md`.
- method: add the Vector user parameter `User.HitDirection` (default (1, 0, 0)); replace the velocity expression by the NS_RobotDebris one driven by User.HitDirection (same speeds, upward part, spread, ThrowScale); keep everything else (offsets table, ExecIndex mesh index, collision, sink). Never edit NS_RobotDebris. Update the doc node.
- done_when: compiles UpToDate, no stack error; user parameter read back; saved; doc updated.
- editor_state: open (launched by the Orchestrator)
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-224-shield-debris-direction]] — parent global task.
