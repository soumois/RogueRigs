---
id: task-224-03-bp-shield-chaser-hitdir
description: "bp: BP_ShieldChaser OnShieldBroken spawns NS_ShieldDebris inactive, sets User.HitDirection from GetShieldDebrisDirection, then activates it."
parent: task-224-shield-debris-direction
kind: task
status: todo
---
# BP_ShieldChaser: pass the throw direction

- goal: the spawned shield debris reads the direction before its burst.
- agent: agent-bp
- depends_on: task-224-02-ns-shield-debris-hitdir
- inputs: `vault/reference/tech/blueprints/enemies/bp-shield-chaser.md` (OnShieldBroken: three SetMaterial then SpawnSystemAtLocation NS_ShieldDebris), `vault/reference/tech/blueprints/fx/bp-shield-debris.md`, `vault/reference/agents/bp-mcp-lessons.md`.
- method: in OnShieldBroken, SpawnSystemAtLocation with Auto Activate off (keep location, rotation, auto destroy), then Set Niagara Variable (Vector3) `User.HitDirection` = GetShieldDebrisDirection (new C++ pure function on the robot, task-224-01), then Activate. Keep the material swaps. Update the doc node.
- done_when: compile and save without error; graph read back.
- editor_state: open (launched by the Orchestrator)
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-224-shield-debris-direction]] — parent global task.
