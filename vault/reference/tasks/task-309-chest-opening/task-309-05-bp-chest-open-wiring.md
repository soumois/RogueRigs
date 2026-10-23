---
id: task-309-05-bp-chest-open-wiring
description: "BP_BonusChest opening wiring (bp): ground ring, rays, sparks and inner light defaults, step 05."
parent: task-309-chest-opening
kind: task
status: todo
---
# BP_BonusChest opening wiring

- goal: In BP_BonusChest: set the InnerLight defaults (colour the company yellow of MI_ChestInterior, intensity and radius to fill the open chest, no shadow); on OnLidSlam spawn NS_HeavySlamImpact at the chest base with User.Radius 200, User.DustColor that yellow at low alpha, User.ChipCount 0 (Q24); on OnLidOpened spawn NS_BonusChestOpen at the open lid; update bp-bonus-chest.
- agent: bp
- depends_on: task-309-01-chest-opening-sequence; task-309-02-bp-chest-interior-material; task-309-03-bp-slam-ring-params; task-309-04-bp-chest-open-fx
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-bonus-chest (vault/reference/tech/blueprints/world/bp-bonus-chest.md), bp-heavy-slam-impact (vault/reference/tech/blueprints/fx/bp-heavy-slam-impact.md), bp-bonus-chest-open (written by task-309-04 under vault/reference/tech/blueprints/fx/), sys-objectives-console-chest (vault/reference/tech/systems/world/sys-objectives/sys-objectives-console-chest.md); files: none; Art/Source/chest/reference/egor-chest-juice.mp4 (techniques F, G)
- expected_output: BP_BonusChest event graph and InnerLight defaults; doc node updated
- done_when: compiles without error; both events and the spawn parameters read back; doc node updated; lint clean; committed
- editor_state: open
- token_ceiling: 25000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-309-chest-opening]] — parent global task.
