---
id: task-196-01-xp-chip-smaller
description: "Big XP chip 15 percent smaller: BP_PickupXPBig template scale 1.8 to 1.53 and its glow disc radius 80 to 68."
parent: task-196-xp-chip-smaller
kind: task
status: done
---
# Smaller big XP chip

- goal: Big XP chip 15 percent smaller: BP_PickupXPBig template scale 1.8 to 1.53 and its glow disc radius 80 to 68.
- agent: bp
- depends_on: none
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), sys-pickups-xp (vault/reference/tech/systems/world/sys-pickups/sys-pickups-xp.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md)
- expected_output: BP_PickupXPBig root template scale (1.53, 1.53, 1.53) (pool restores the spawned root template scale, sys-pickups-xp); MI_XPGlowDisc_Chip Radius 68 (world cm, not scaled by the actor); TrailFX relative scale unchanged (it follows the root); saved by explicit path; bp-pickup-xp updated.
- done_when: Blueprint compiles; scale and Radius read back; docs updated; lint clean; committed by path.
- editor_state: open
- token_ceiling: open
- model: sonnet
- tokens: input 34, cache write 39159, cache read 443243, output 462
- runs: 1
- milestone: M1
- result: BP_PickupXPBig root template scale 1.53 (was 1.8) and MI_XPGlowDisc_Chip Radius 68 (was 80), read back and saved; TrailFX relative 0.3 unchanged (effective 0.459); docs updated; not seen in PIE; commit e73816a3

## Relations
- [[task-196-xp-chip-smaller]] — parent global task.
