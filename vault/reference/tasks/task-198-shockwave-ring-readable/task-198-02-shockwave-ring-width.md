---
id: task-198-02-shockwave-ring-width
description: "BP: the shockwave ring width becomes a material scalar parameter RingWidth, default 60 uu; doc updated."
parent: task-198-shockwave-ring-readable
kind: task
status: todo
---
# Shockwave ring width

- goal: in `/Game/RogueRigs/FX/M_ExplosionShockwave` the ring width hard-coded at 24 uu in the progress-ring Custom node becomes a scalar parameter `RingWidth` (default 60 uu, provisional, owner 2026-10-16), still inner-aligned so the ring is not cut at the box edge; `MI_ExplosionShockwave` keeps the default; doc `bp-explosion-shockwave` updated.
- agent: bp
- depends_on: none
- inputs: bp-explosion-shockwave (vault/reference/tech/blueprints/fx/bp-explosion-shockwave.md), sys-ground-telegraph (vault/reference/tech/systems/combat/sys-ground-telegraph.md), sys-explosions (vault/reference/tech/systems/combat/sys-explosions.md), color-code (vault/reference/gdd/art-audio/color-code.md), agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: material and instance saved by explicit path; doc; one commit.
- done_when: material compiles; `RingWidth` read back 60 on the material and the instance; doc updated; committed by path.
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-198-shockwave-ring-readable]] — parent global task.
