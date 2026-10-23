---
id: task-298-01-bp-shooter-charge-ns
description: "Create NS_ShooterCharge, a red charge-up glow at a point growing over 0.6 s, with its doc node (bp)."
parent: task-298-shooter-charge-fx
kind: task
status: in-progress
---
# NS_ShooterCharge

- goal: New Niagara system NS_ShooterCharge: small red core glow growing over 0.6 s plus a few sparks drawn inward, about 15 to 30 cm across at full charge, local space so it follows the muzzle, ends when deactivated; red of the laser and shots; doc node bp-shooter-charge under blueprints-fx-index.
- agent: bp
- depends_on: none
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), blueprints-fx-index (vault/reference/tech/blueprints/fx/blueprints-fx-index.md), bp-shooter (vault/reference/tech/blueprints/enemies/bp-shooter.md), color-code (vault/reference/gdd/art-audio/color-code.md), enemy-shooter (vault/reference/gdd/enemies/enemy-shooter.md); files: none
- expected_output: /Game/RogueRigs/FX/NS_ShooterCharge, doc node bp-shooter-charge
- done_when: compiles without error; values read back; doc node and index relation added; lint clean
- editor_state: open (owner editor, MCP route, do not close it)
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-298-shooter-charge-fx]] — parent global task.
