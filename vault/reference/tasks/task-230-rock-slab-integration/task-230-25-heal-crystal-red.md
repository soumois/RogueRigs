---
id: task-230-25-heal-crystal-red
description: "Heal crystals read red, not orange-red, per color-code."
parent: task-230-rock-slab-integration
kind: task
status: in-progress
---
# Heal crystals read red, not orange-red, per color-code

- goal: heal ore crystals read red with no orange (owner 2026-10-19, after the 230-19 captures (Saved/RockSlab/after3/): rs_h_crystals_00 shows orange-red glow). MI_HealCrystal_Bright (and _Dark if used) Color/GlowColor (1.0, 0.03, 0.03), glow 12: find why it reads orange (bloom, tonemapper, emissive saturation) and adjust so it reads red next to violet Duskite.
- agent: bp
- depends_on: task-230-21-rock-rim-tone-restore
- inputs: vault/reference/tech/blueprints/fx/bp-pickup-ore.md, vault/reference/gdd/art-audio/color-code.md, vault/reference/agents/bp-mcp-lessons.md; capture Saved/RockSlab/after3/rs_h_crystals_00.png
- expected_output: MI_HealCrystal values changed, doc updated.
- done_when: saved; doc updated; committed; look checked in task-230-26.
- editor_state: open
- token_ceiling: 20000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
