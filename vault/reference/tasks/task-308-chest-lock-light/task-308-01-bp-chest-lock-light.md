---
id: task-308-01-bp-chest-lock-light
description: "MI_ChestLockLight on the chest latch (bp): bonus chest lock light states, step 01."
parent: task-308-chest-lock-light
kind: task
status: done
---
# MI_ChestLockLight on the chest latch

- goal: Create MI_ChestLockLight, child of M_ConsoleLamp (LampColor red, BlinkRate 2, emissive as the console lamp), and assign it to the LockLight slot of the Latch component of BP_BonusChest; record it in bp-bonus-chest. M_Chest has no emissive input (task-306-03, bp-bonus-chest): the glow comes only from MI_ChestLockLight on the LockLight slot; M_Chest stays unchanged.
- agent: bp
- depends_on: task-306-03-bp-chest-import; task-301-04-bp-console-c2-import (M_ConsoleLamp)
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-bonus-chest (vault/reference/tech/blueprints/world/bp-bonus-chest.md), bp-objective-console (vault/reference/tech/blueprints/world/bp-objective-console.md), color-code (vault/reference/gdd/art-audio/color-code.md); files: none
- expected_output: /Game/RogueRigs/ MI_ChestLockLight; BP_BonusChest slot assignment; doc node updated
- done_when: compiles without error; slot assignment and parameter values read back; doc node updated; lint clean; committed
- editor_state: open
- token_ceiling: 1.0 M units
- model: sonnet
- tokens: input 26, cache write 39504, cache read 353961, output 6486
- usage: turns 13, context start 11k peak 40k, resumes 0, 1 min, units 0.12 M
- runs: 1
- milestone: M2
- started: 2026-10-29T17:38:20Z
- result: MI_ChestLockLight (parent M_ConsoleLamp, LampColor 1,0.05,0.03, BlinkRate 2, BlinkAmount 1) on Latch slot 1 of BP_BonusChest; compiled; bp-bonus-chest updated; commit 50a48feb. Look unverified (no capture).

## Relations
- [[task-308-chest-lock-light]] — parent global task.
