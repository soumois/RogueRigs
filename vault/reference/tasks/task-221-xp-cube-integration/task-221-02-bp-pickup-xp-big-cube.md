---
id: task-221-02-bp-pickup-xp-big-cube
description: "bp: BP_PickupXPBig PickupMesh becomes SM_XPCubeBig with its materials, RestOrientation Full; doc node bp-pickup-xp updated."
parent: task-221-xp-cube-integration
kind: task
status: todo
---
# BP_PickupXPBig: cube

- goal: the big XP pickup in game is the data cube.
- agent: agent-bp
- depends_on: task-221-01-xp-cube-import
- inputs: `vault/reference/tech/blueprints/fx/bp-pickup-xp.md`, `vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md` (rest orientation), `vault/reference/agents/bp-mcp-lessons.md`, the result of task-221-01.
- method: BP_PickupXPBig root PickupMesh = SM_XPCubeBig; OverrideMaterials = the five cube slots in order (empty the array first, then set the five entries: lesson of task-220-03); template scale 1.53 kept; RestOrientation = Full; TrailFX, sparkle, glow fade, pop values unchanged. Do not touch BP_PickupXP. Update the doc node (current state first, history of the chip kept short).
- done_when: compile and save without error; mesh, materials, scale and RestOrientation read back.
- editor_state: open (launched by the Orchestrator)
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-221-xp-cube-integration]] — parent global task.
