---
id: task-291-04-bp-shield-v2-v1-clips
description: "Import the 291-02 bake as a new texture for the shield chaser v2 master and instances, RunRate in sync (bp)."
parent: task-291-chaser-v2-v1-anims
kind: task
status: done
---
# Shield chaser v2 v1-clips texture in Unreal

- goal: Import the 291-02 EXR as T_ShieldChaserV2AnimV1Clips; repoint the TextureObject of M_ShieldChaserV2_Anim and the AnimTex of the two shield chaser v2 eye instances; reimport the mesh as a new asset and set it on BP_ShieldChaserV2 BodyMesh (slot overrides 0 body, 1 eye, 2 shield, 3 emblem) only if 291-02 changed the FBX; RunRate from 291-02 on every shield chaser v2 instance (Body, BodyPlain, Shield, Emblem, Hidden, eye, eye plain).
- agent: bp
- depends_on: task-291-03-bp-chaser-v2-v1-clips
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-shield-chaser-v2 (vault/reference/tech/blueprints/enemies/bp-shield-chaser-v2.md), bp-robot-meshy-material (vault/reference/tech/blueprints/enemies/bp-robot-meshy-material.md), robot-parts-bake-pipeline (vault/reference/tech/pipelines/robot-parts-bake-pipeline.md); files: report of 291-02
- expected_output: T_ShieldChaserV2AnimV1Clips (and mesh if needed); master and instances updated; bp-shield-chaser-v2 and bp-robot-meshy-material updated
- done_when: texture 20 x 62 and settings read back; master recompiled without error; RunRate and ClipOffset read back on every instance; BP_ShieldChaserV2 compiled and saved; v1 assets untouched; docs updated; lint clean
- editor_state: open (owner editor, MCP route, do not close it)
- token_ceiling: 40000
- model: sonnet
- tokens: input 48, cache write 58131, cache read 935885, output 14796
- runs: 1
- milestone: M2
- result: T_ShieldChaserV2AnimV1Clips (20x62, settings read back) in M_ShieldChaserV2_Anim and both eye instances; new mesh SM_ShieldChaserV2_Meshy_AnimV1Clips (7134 tris, 4 slots) on BP_ShieldChaserV2 BodyMesh, compiled and saved; RunRate 0.7 on all 7 instances, ClipOffset 0/2 read back; old mesh and texture kept; docs updated; commit 05e2aff0.

## Relations
- [[task-291-chaser-v2-v1-anims]] — parent global task.
