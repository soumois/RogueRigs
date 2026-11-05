---
id: task-448-02-heavy-puff-check
description: "Bp: The heavy death puff is read in the simulation on a real kill, and fixed only if its compiled values are wrong."
parent: task-448-death-puffs-sim-check
kind: task
status: todo
---
# Check the robot death puffs in the simulation: bp

- goal: The heavy death puff is read in the simulation on a real kill, and fixed only if its compiled values are wrong.
- agent: bp
- depends_on: task-448-01-robot-puff-check
- inputs: task-436-robot-death-particle (vault/reference/tasks/task-436-robot-death-particle.md), bp-heavy-death-puff (vault/reference/tech/blueprints/fx/bp-heavy-death-puff.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: see done_when
- done_when: Particle dump from a real heavy kill in PIE, per emitter, given in the result, matching the asset; asset unchanged unless the dump was wrong; no LogNiagara error.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-448-death-puffs-sim-check]] — parent global task.
