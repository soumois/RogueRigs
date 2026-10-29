---
id: task-309-02-bp-chest-interior-material
description: "MI_ChestInterior on the Interior slot (bp): bonus chest opening, emissive interior, step 02."
parent: task-309-chest-opening
kind: task
status: done
---
# MI_ChestInterior on the Interior slot

- goal: Create MI_ChestInterior (M_Chest has no emissive input, bp-bonus-chest: use or create an emissive parent, M_Chest stays unchanged): emissive company yellow sampled from the chest logo plate of the Meshy texture (Q23), scalar InteriorGlow 0 by default (driven by task-309-01); assign it to the Interior slot of the chest body in BP_BonusChest; record the linear colour value in bp-bonus-chest and report it.
- agent: bp
- depends_on: task-306-03-bp-chest-import
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-bonus-chest (vault/reference/tech/blueprints/world/bp-bonus-chest.md), color-code (vault/reference/gdd/art-audio/color-code.md), kessler-voss-logo (vault/reference/gdd/art-audio/kessler-voss-logo.md), art-prompts-chest (vault/reference/gdd/art-audio/art-prompts-chest.md); files: Art/Source/chest/clean/ (texture); Art/Source/chest/reference/egor-chest-juice.mp4 (technique F)
- expected_output: /Game/RogueRigs/ MI_ChestInterior (and its parent material if none fits); slot assignment; doc node updated
- done_when: compiles without error; slot assignment, colour and InteriorGlow read back; doc node updated; lint clean; committed
- editor_state: open
- token_ceiling: 1.0 M units
- model: sonnet
- tokens: input 46, cache write 62390, cache read 990400, output 16799
- usage: turns 23, context start 12k peak 62k, resumes 0, 2 min, units 0.26 M
- runs: 1
- milestone: M2
- started: 2026-10-29T18:57:21Z
- result: Done: M_ChestInterior (emissive parent) + MI_ChestInterior, InteriorGlow 0 default, EmissiveColor linear (1,0.58,0) company yellow ~#FFC800 (logo texture has no yellow: not sampled, deviation Q23); assigned to Mesh OverrideMaterials[1] of BP_BonusChest. Look unverified.

## Relations
- [[task-309-chest-opening]] — parent global task.
