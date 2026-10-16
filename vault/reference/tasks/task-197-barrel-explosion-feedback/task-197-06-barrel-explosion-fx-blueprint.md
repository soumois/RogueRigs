---
id: task-197-06-barrel-explosion-fx-blueprint
description: "BP: BP_ExplosionFX_Barrel with the burst and shockwave set, ExplosionStats Barrel row pointed to it, PIE read-back; doc."
parent: task-197-barrel-explosion-feedback
kind: task
status: todo
---
# Barrel explosion FX Blueprint

- goal: new `BP_ExplosionFX_Barrel` in `/Game/RogueRigs/Blueprints/FX/`, child of `BP_ExplosionFX` (sphere and material kept), with the Niagara burst `NS_BarrelExplosion` and the shockwave material instance from task-197-05 set; `Content/RogueRigs/Data/Tables/ExplosionStats.csv` row `Barrel` `FXClass` points to it, row `Kamikaze` unchanged; PIE check: a fuse lit through the BlueprintCallable `ARRArenaGrid::DestroyCell` on a barrel cell ends in an explosion, the spawned FX actor has its Niagara component active and its shockwave progress rising (values read back); new doc node `bp-explosion-fx-barrel`, and `bp-explosion-fx`, `sys-explosions` (data line) and `blueprints-fx-index` updated.
- agent: bp
- depends_on: task-197-03-barrel-fuse-blink-material, task-197-04-barrel-explosion-niagara, task-197-05-shockwave-decal-material
- inputs: barrel (vault/reference/gdd/terrain/barrel.md), explosions (vault/reference/gdd/terrain/explosions.md), sys-explosions (vault/reference/tech/systems/combat/sys-explosions.md), bp-explosion-fx (vault/reference/tech/blueprints/fx/bp-explosion-fx.md), blueprints-fx-index (vault/reference/tech/blueprints/fx/blueprints-fx-index.md), agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: Blueprint asset, CSV row, docs; one commit.
- done_when: Blueprint compiles; CSV row read back; PIE read-backs pass (barrel FX spawned with burst and ring; a kamikaze explosion still spawns `BP_ExplosionFX`); docs updated; committed by path.
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-197-barrel-explosion-feedback]] — parent global task.
