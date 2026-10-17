---
id: task-222-02-shield-debris-import
description: "bp: import the five shield debris meshes and give them a static debris material from the shield textures."
parent: task-222-shield-break-debris
kind: task
status: in-progress
---
# Shield debris: import and material

- goal: `/Game/RogueRigs/Art/ShieldChaser/Debris/SM_ShieldDebris_*` with `M_ShieldDebris` (and an emblem variant), ready for Niagara.
- agent: agent-bp
- depends_on: task-222-01-shield-debris-pieces
- inputs: `Art/Source/shield-chaser/debris/` (FBX and json); `vault/reference/tech/pipelines/chaser-meshy-pipeline.md` (Unreal section, debris import: StaticMeshTools.import_file, no collision, Nanite off, one LOD); `vault/reference/tech/blueprints/fx/bp-robot-debris.md`; textures already imported in `/Game/RogueRigs/Art/ShieldChaser/` (T_ChaserShield_BaseColor, T_ChaserShield_Emblem); `vault/reference/agents/bp-mcp-lessons.md`.
- method: import the five pieces like the SM_ChaserDebris_* pieces. `M_ShieldDebris`: opaque, default lit, base colour from T_ChaserShield_BaseColor, no WPO, `bUsedWithNiagaraMeshParticles` on (as M_ChaserDebris); `M_ShieldDebris_Emblem`: Masked with the emblem texture alpha, same flag. Assign to the slots. Add the assets to the debris lines of the pipeline node.
- done_when: assets saved; slots and flags read back.
- editor_state: open (launched by the Orchestrator)
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-222-shield-break-debris]] — parent global task.
