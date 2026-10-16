---
id: task-199-01-barrel-burst-bigger
description: "NS_BarrelExplosion bigger: wider fireball, about twice the sparks, more visible smoke, still within 1.5 s."
parent: task-199-barrel-burst-bigger
kind: task
status: todo
---
# Barrel burst bigger

- goal: NS_BarrelExplosion bigger: wider fireball, about twice the sparks, more visible smoke, still within 1.5 s.
- agent: bp
- depends_on: none
- inputs: barrel (vault/reference/gdd/terrain/barrel.md), bp-barrel-explosion (vault/reference/tech/blueprints/fx/bp-barrel-explosion.md), sys-explosions (vault/reference/tech/systems/combat/sys-explosions.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: `/Game/RogueRigs/FX/NS_BarrelExplosion` (and its sprite material if needed) saved by explicit path; doc `bp-barrel-explosion`; one commit.
- done_when: system compiles; new counts, sizes and colours read back and written in the doc (provisional, owner 2026-10-16); last particle ends before 1.5 s; committed by path.
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-199-barrel-burst-bigger]] — parent global task.
