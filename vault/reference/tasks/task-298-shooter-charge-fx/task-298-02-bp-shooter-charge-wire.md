---
id: task-298-02-bp-shooter-charge-wire
description: "Attach NS_ShooterCharge to the shooter MuzzlePoint, started on OnAimStart, stopped on OnFire and OnAimEnd (bp)."
parent: task-298-shooter-charge-fx
kind: task
status: done
---
# BP_Shooter plays the charge-up

- goal: BP_Shooter: a Niagara component ChargeFX (NS_ShooterCharge, auto activate off) attached to MuzzlePoint; OnAimStart Activate (reset), OnFire and OnAimEnd Deactivate; visuals only.
- agent: bp
- depends_on: task-298-01-bp-shooter-charge-ns
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-shooter (vault/reference/tech/blueprints/enemies/bp-shooter.md), sys-enemies-shooter (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-shooter.md); files: report of 298-01
- expected_output: BP_Shooter updated; bp-shooter updated (before, after, revert)
- done_when: component and three events read back; compiled and saved; doc updated; lint clean
- editor_state: open (owner editor, MCP route, do not close it)
- token_ceiling: 30000
- model: sonnet
- tokens: input 48, cache write 58849, cache read 969720, output 10950
- runs: 1
- milestone: M2
- result: BP_Shooter (c9a2ded2): ChargeFX NiagaraComponent (NS_ShooterCharge, auto activate off) on MuzzlePoint; OnAimStart Activate(reset), OnFire and OnAimEnd Deactivate; compiled, saved, bp-shooter and bp-shooter-charge docs updated. Not seen in PIE.

## Relations
- [[task-298-shooter-charge-fx]] — parent global task.
