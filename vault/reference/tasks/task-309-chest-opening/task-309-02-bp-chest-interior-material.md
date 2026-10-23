---
id: task-309-02-bp-chest-interior-material
description: "MI_ChestInterior on the Interior slot (bp): bonus chest opening, emissive interior, step 02."
parent: task-309-chest-opening
kind: task
status: todo
---
# MI_ChestInterior on the Interior slot

- goal: Create MI_ChestInterior: emissive company yellow sampled from the chest logo plate of the Meshy texture (Q23), scalar InteriorGlow 0 by default (driven by task-309-01); assign it to the Interior slot of the chest body in BP_BonusChest; record the linear colour value in bp-bonus-chest and report it.
- agent: bp
- depends_on: task-306-03-bp-chest-import
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-bonus-chest (vault/reference/tech/blueprints/world/bp-bonus-chest.md), color-code (vault/reference/gdd/art-audio/color-code.md), kessler-voss-logo (vault/reference/gdd/art-audio/kessler-voss-logo.md), art-prompts-chest (vault/reference/gdd/art-audio/art-prompts-chest.md); files: Art/Source/chest/clean/ (texture); Art/Source/chest/reference/egor-chest-juice.mp4 (technique F)
- expected_output: /Game/RogueRigs/ MI_ChestInterior (and its parent material if none fits); slot assignment; doc node updated
- done_when: compiles without error; slot assignment, colour and InteriorGlow read back; doc node updated; lint clean; committed
- editor_state: open
- token_ceiling: 25000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-309-chest-opening]] — parent global task.
