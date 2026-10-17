---
id: task-224-02-ns-shield-debris-hitdir
description: "bp: NS_ShieldDebris gets User.HitDirection back and throws its pieces along it with the NS_RobotDebris velocity setup."
parent: task-224-shield-debris-direction
kind: task
status: done
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
- model: sonnet
- tokens: input 54, cache write 49173, cache read 918894, output 829
- runs: 1
- milestone: M1
- result: Commit 88841d1c: NS_ShieldDebris user parameter User.HitDirection (Vector, default (1, 0, 0)); particle velocity expression now identical to NS_RobotDebris (HitDirection rotated by ThrowAngle, 250-300 speed, upward 269-275, spread 100, ThrowScale); offsets, ExecIndex, collision, sink unchanged; compiles UpToDate, 0 stack errors. The spawner must set User.HitDirection (default is world +X).

## Relations
- [[task-224-shield-debris-direction]] — parent global task.
