---
id: task-219-02-shield-chaser-material
description: "bp: import the combined shield chaser mesh and textures, build M_ShieldChaser_Anim with the 4-clip layout and its five instances."
parent: task-219-shield-chaser-integration
kind: task
status: todo
---
# Shield chaser: import and material

- goal: the combined mesh, its textures and one parent material with its instances, ready for BP_ShieldChaser.
- agent: agent-bp
- depends_on: task-219-01-shield-clips-bake
- inputs: outputs of task-219-01 in `Art/Meshes/shield-chaser/` and its report; `Art/Meshes/ChaserShield/T_ChaserShield_BaseColor.png` and `T_ChaserShield_Emblem.png`; `vault/reference/tech/pipelines/chaser-meshy-pipeline.md` (Unreal section: build settings, UV channel 2 check, texture settings), `vault/reference/tech/blueprints/enemies/bp-chaser.md`, `vault/reference/tech/pipelines/tool-unreal-reimport.md`, `vault/reference/agents/bp-mcp-lessons.md`.
- method: folder `/Game/RogueRigs/Art/ShieldChaser/`.
  - Import `SM_ShieldChaser_Meshy_Anim` with the settings of SM_Chaser_Meshy_Anim (Generate Lightmap UVs off, LightMapCoordinateIndex 1, full precision UVs, extended bounds large enough for the bash reach, no collision, Nanite off); check channel 2 on RENDER_DATA (shield corners 0.45).
  - `T_ShieldChaserAnim` with the settings of T_ChaserAnim (TC_HDR, no mips, Nearest, sRGB off). Shield base colour sRGB; emblem texture with alpha.
  - `M_ShieldChaser_Anim`: a copy of M_Chaser_Meshy_Anim (never edit the chaser material). Its WPO reads the 4-clip layout: clip = Custom Primitive Data 0 + scalar ClipOffset; start rows 0, 16, 31, 47; frames 16, 15, 16, 15; texture height 62; RunRate on clips 0 and 2 only. Base colour from a texture parameter; blend Masked with OpacityMask = texture alpha x scalar Visible (default 1); glow on red texels as the chaser.
  - Instances: MI_ShieldChaser_Body (chaser base colour, ClipOffset 0), MI_ShieldChaser_BodyPlain (ClipOffset 2), MI_ShieldChaser_Shield (shield base colour), MI_ShieldChaser_Emblem (emblem texture), MI_ShieldChaser_Hidden (Visible 0).
  - Doc: add these assets to the Unreal section of `vault/reference/tech/pipelines/chaser-meshy-pipeline.md`.
- done_when: assets saved, material compiles, channel 2 check passes.
- editor_state: open (launched by the Orchestrator)
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-219-shield-chaser-integration]] — parent global task.
