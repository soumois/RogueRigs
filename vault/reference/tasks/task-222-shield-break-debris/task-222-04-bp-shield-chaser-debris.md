---
id: task-222-04-bp-shield-chaser-debris
description: "bp: BP_ShieldChaser OnShieldBroken also spawns NS_ShieldDebris at the actor transform; doc node updated."
parent: task-222-shield-break-debris
kind: task
status: todo
---
# BP_ShieldChaser: spawn the shield debris

- goal: the shield break throws the fragments.
- agent: agent-bp
- depends_on: task-222-03-ns-shield-debris
- inputs: `vault/reference/tech/blueprints/enemies/bp-shield-chaser.md` (current graph: OnShieldBroken swaps three materials), `vault/reference/tech/blueprints/fx/bp-shield-debris.md` (created by task-222-03), `vault/reference/agents/bp-mcp-lessons.md`.
- method: in the existing OnShieldBroken chain, after the material swaps, SpawnSystemAtLocation NS_ShieldDebris at the actor location (feet level, same origin as the json offsets) with the actor rotation, auto destroy on; keep everything else. Update the doc node.
- done_when: compile and save without error; graph read back.
- editor_state: open (launched by the Orchestrator)
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-222-shield-break-debris]] — parent global task.
