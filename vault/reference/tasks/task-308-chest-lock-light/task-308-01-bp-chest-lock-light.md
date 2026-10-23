---
id: task-308-01-bp-chest-lock-light
description: "MI_ChestLockLight on the chest latch (bp): bonus chest lock light states, step 01."
parent: task-308-chest-lock-light
kind: task
status: todo
---
# MI_ChestLockLight on the chest latch

- goal: Create MI_ChestLockLight, child of M_ConsoleLamp (LampColor red, BlinkRate 2, emissive as the console lamp), and assign it to the LockLight slot of the Latch component of BP_BonusChest; record it in bp-bonus-chest.
- agent: bp
- depends_on: task-306-03-bp-chest-import; task-301-04-bp-console-c2-import (M_ConsoleLamp)
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-bonus-chest (vault/reference/tech/blueprints/world/bp-bonus-chest.md), bp-objective-console (vault/reference/tech/blueprints/world/bp-objective-console.md), color-code (vault/reference/gdd/art-audio/color-code.md); files: none
- expected_output: /Game/RogueRigs/ MI_ChestLockLight; BP_BonusChest slot assignment; doc node updated
- done_when: compiles without error; slot assignment and parameter values read back; doc node updated; lint clean; committed
- editor_state: open
- token_ceiling: 25000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-308-chest-lock-light]] — parent global task.
