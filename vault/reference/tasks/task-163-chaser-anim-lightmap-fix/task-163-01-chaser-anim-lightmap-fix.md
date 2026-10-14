---
id: task-163-01-chaser-anim-lightmap-fix
description: "Fix the chaser animation: the engine lightmap UVs overwrite the part index in UV channel 2."
parent: task-163-chaser-anim-lightmap-fix
kind: task
status: in-progress
---
# Chaser: keep the part index UV

- goal: Fix the chaser animation: the engine lightmap UVs overwrite the part index in UV channel 2.
- agent: cpp
- depends_on: none
- inputs: sys-enemies-chaser (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-chaser.md), bp-chaser (vault/reference/tech/blueprints/enemies/bp-chaser.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), tool-unreal-reimport (vault/reference/tech/pipelines/tool-unreal-reimport.md)
- expected_output: On /Game/RogueRigs/Art/Chaser/SM_Chaser_Meshy_Anim: build settings generate_lightmap_u_vs False, light_map_coordinate_index 1 (the LightMap channel of the source), use_full_precision_u_vs True, rebuilt and saved (only this asset); a read-only check of the RENDER_DATA UV channel 2 over all three corners of every triangle (ten bins at (i + 0.5) / 10, zero mixed triangles); a fresh game run with rr.Debug.Shots of the chaser running and attacking (at least 6 shots each, opened and described); the docs bp-chaser updated (the asset keeps those build settings and why: never let the engine generate a lightmap into the PartId channel)
- done_when: The render data check reports ten bins and 0 mixed triangles; the shots show intact parts moving (arms, legs and torso separate and coherent) in the run and the attack, or, if not, a precise description of what is still wrong; only that asset changed; docs updated; committed by name; the editor stays closed
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-163-chaser-anim-lightmap-fix]] — parent global task.
