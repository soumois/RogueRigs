---
id: task-197-05-shockwave-decal-material
description: "BP: decal material for the orange explosion shockwave ring on the ground, driven by radius, progress and opacity; doc."
parent: task-197-barrel-explosion-feedback
kind: task
status: done
---
# Explosion shockwave ring material

- goal: new deferred decal material `M_ExplosionShockwave` and an orange instance in `/Game/RogueRigs/FX/`: a sharp orange ring (provisional width written in the doc) at radius x progress, fading with the opacity parameter, no fill; parameter names as in task-197-02; same projection rules as `M_GroundTelegraph`; new doc node in `tech/blueprints/fx/`, `blueprints-fx-index` updated.
- agent: bp
- depends_on: task-197-02-explosion-fx-burst-shockwave
- inputs: ground-telegraph (vault/reference/gdd/art-audio/ground-telegraph.md), sys-ground-telegraph (vault/reference/tech/systems/combat/sys-ground-telegraph.md), sys-explosions (vault/reference/tech/systems/combat/sys-explosions.md), explosions (vault/reference/gdd/terrain/explosions.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), blueprints-fx-index (vault/reference/tech/blueprints/fx/blueprints-fx-index.md), agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: material and instance saved by explicit path; doc node; one commit.
- done_when: material compiles as a deferred decal; parameter names match `sys-explosions`; doc and index updated; committed by path.
- editor_state: open
- token_ceiling: open
- model: sonnet
- tokens: input 30, cache write 42069, cache read 428400, output 9038
- runs: 1
- milestone: M1
- result: M_ExplosionShockwave (deferred decal, translucent, unlit) and orange MI_ExplosionShockwave; Radius, Progress, Opacity; solid ring 24 uu provisional at radius x progress; doc bp-explosion-shockwave; commit 8433d794

## Relations
- [[task-197-barrel-explosion-feedback]] — parent global task.
