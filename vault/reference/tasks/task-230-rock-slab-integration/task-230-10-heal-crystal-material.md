---
id: task-230-10-heal-crystal-material
description: "Red heal crystal material instance for the crystal clusters on heal veins."
parent: task-230-rock-slab-integration
kind: task
status: todo
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
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
