---
id: task-262-01-deposit-crystal-volume
description: "The Duskite and heal deposit crystals read as volumes: capped emission peak, height gradient and rim light, textures allowed if needed."
parent: task-262-deposit-crystal-volume
kind: task
status: done
---
# Deposit crystal volume

- goal: The Duskite and heal deposit crystals read as volumes: capped emission peak, height gradient and rim light, textures allowed if needed.
- agent: bp
- depends_on: none
- inputs: bp-pickup-ore (vault/reference/tech/blueprints/fx/bp-pickup-ore.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), bp-pickup-heal (vault/reference/tech/blueprints/fx/bp-pickup-heal.md), sys-grid-deposits (vault/reference/tech/systems/world/sys-grid/sys-grid-deposits.md), bp-arena-grid (vault/reference/tech/blueprints/world/bp-arena-grid.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: A material for the deposit crystals (child or sibling of M_XPNutGlow keeping the PulsePhase and PulseRate inputs of task-257 at indices 4 and 5): emission peak capped so the crystal stays violet (heal: red) at the pulse peak, emission lower at the base and higher at the tip (height gradient), a Fresnel rim, dark base colour lit by the scene so each facet keeps its tone; instances for Duskite (MI_DuskiteDeposit_Crystal) and heal crystals (MI_HealCrystal_*) switched to it; a small texture (gradient, AO or normal) only if the procedural version is not enough, stated and documented; the ore pickups unchanged; doc nodes updated; values provisional, owner tunes by eye.
- done_when: Material compiles; the deposit instances use it (read back); emissive peak and rest values read back; the three deposit variants keep their slots; ore pickup instances unchanged; pulse desync inputs still wired; docs updated; lint clean.
- editor_state: open (editor + MCP)
- token_ceiling: 150k
- model: sonnet
- tokens: input 56, cache write 109887, cache read 2155400, output 4027
- runs: 1
- milestone: M1
- result: New M_DepositCrystalGlow (duplicate of M_XPNutGlow, which is untouched): same pulse inputs (per-instance 4 and 5), emission capped (GlowAmount 2, PulseAmount 0.5; peak about (2.5, 0.45, 4.5)), height gradient (GradientBase 0.35 over 70 uu) and Fresnel rim (power 3, amount 0.5), Roughness 0.4, no texture. MI_DuskiteDeposit_Crystal reparented (violet values kept); new MI_HealDepositCrystal (red, GlowAmount 1.5) on the heal deposit crystal slots; MI_HealCrystal_* kept for the rock crystal clusters; ore pickups unchanged. Compiles; node bp-deposit-crystal-material with tuning knobs. Not seen on screen.

## Relations
- [[task-262-deposit-crystal-volume]] — parent global task.
