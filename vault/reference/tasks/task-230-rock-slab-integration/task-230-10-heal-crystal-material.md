---
id: task-230-10-heal-crystal-material
description: "Red heal crystal material instance for the crystal clusters on heal veins."
parent: task-230-rock-slab-integration
kind: task
status: done
---
# Red heal crystal material instance for the crystal clusters on heal veins

- goal: Red heal crystal material instance for the crystal clusters on heal veins.
- agent: bp
- depends_on: task-230-09-rock-slab-material
- inputs: vault/reference/gdd/art-audio/color-code.md, vault/reference/agents/bp-mcp-lessons.md; MI_OreChunk_Bright and MI_OreChunk_Dark (ore chunk materials)
- expected_output: MI_HealCrystal_Bright and _Dark from the ore chunk parent, red per color-code (no orange); doc updated.
- done_when: instances saved; doc updated; committed.
- editor_state: open
- token_ceiling: 30000
- model: sonnet
- tokens: input 28, cache write 26556, cache read 279015, output 456
- runs: 1
- milestone: M2
- result: Commit 7d226bb5. MI_HealCrystal_Bright (1.0, 0.03, 0.03, glow 12) and MI_HealCrystal_Dark (0.5, 0.01, 0.01, glow 6) duplicated from the ore chunk instances in /Game/RogueRigs/Art/OreChunk/; red values provisional; doc bp-pickup-ore updated.

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
