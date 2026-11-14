---
id: task-543-01-debris-colour
description: "Bp: Retint the rock destruction debris to the rock colours, no glow"
parent: task-543-wall-debris-less-white
kind: task
status: done
---
# Wall destruction particles: rock colours instead of near white: bp

- goal: Retint the rock destruction debris to the rock colours, no glow
- agent: bp
- depends_on: none
- inputs: color-code (vault/reference/gdd/art-audio/color-code.md), Saved/Scratch/owner-ui-batch-2026-11-13/3.png
- expected_output: see done_when
- done_when: A1 to A3 hold.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-543
- usage: included in the executor run of task-543
- runs: 1
- milestone: M2
- started: 2026-11-13T23:32:36Z
- waiting_for: none
- result: Done (e47ae4d1). Real effects found by code and asset registry: NS_DigDebris (drone breaks a block, the white flakes of image 3) and NS_ExplosionWallDebris, both M_DigChip unlit; rock chips were (0.35, 0.28, 0.5) and rendered near white. RRBlockColors::RockDebrisTones lilac (0.12, 0.075, 0.2), mid violet (0.06, 0.015, 0.15), dark violet (0.025, 0.006, 0.06), provisional; NS_DigDebris gets User.ChipColor2/3 and an InitializeParticle Color HLSL picking one tone per particle (ExecIndex mod 3); the drone sets the three (ore, heal ore, barrel: their colour three times); explosion wall debris picks a random tone per rock particle. Count, size, speed, lifetime unchanged; NS_DigImpact chips and ore colours unchanged. Tests RogueRigs.Dig.DigDebris + RogueRigs.Explosions.WallDebris 4/4 pass, DigDebrisAsset now reads the particle colours: 4 lilac, 4 mid, 4 dark. Pitfall: the MCP Niagara HLSL edit stayed out of the compiled script until another stack edit followed it (unreal-mcp-quirks); first tones too pale, then darkened after the capture. Executor run over the light budget (40 turns): Niagara compile quirk and three capture rounds.

## Relations
- [[task-543-wall-debris-less-white]] — parent global task.
