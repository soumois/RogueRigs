---
id: task-197-04-barrel-explosion-niagara
description: "BP: NS_BarrelExplosion, a one-shot burst (flash, orange fireball, sparks, a little smoke) scaled by the radius user parameter; doc node."
parent: task-197-barrel-explosion-feedback
kind: task
status: in-progress
---
# Barrel explosion Niagara burst

- goal: new Niagara system `NS_BarrelExplosion` in `/Game/RogueRigs/FX/`: one-shot, a short bright flash, an orange fireball, sparks thrown outward and a little dark smoke, all sized from the float radius user parameter named in task-197-02 (fills about the visual radius), done in about 1.5 s, no collision, the cheapest of CPU or GPU; colours per `color-code` (orange = explodes); provisional counts written in the doc; new doc node in `tech/blueprints/fx/`, `blueprints-fx-index` updated.
- agent: bp
- depends_on: task-197-02-explosion-fx-burst-shockwave
- inputs: barrel (vault/reference/gdd/terrain/barrel.md), explosions (vault/reference/gdd/terrain/explosions.md), sys-explosions (vault/reference/tech/systems/combat/sys-explosions.md), bp-explosion-fx (vault/reference/tech/blueprints/fx/bp-explosion-fx.md), bp-robot-death-puff (vault/reference/tech/blueprints/fx/bp-robot-death-puff.md), blueprints-fx-index (vault/reference/tech/blueprints/fx/blueprints-fx-index.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: Niagara asset saved by explicit path; doc node; one commit.
- done_when: system compiles; user parameter name matches `sys-explosions`; emitters, counts and lifetime read back; doc and index updated; committed by path.
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-197-barrel-explosion-feedback]] — parent global task.
