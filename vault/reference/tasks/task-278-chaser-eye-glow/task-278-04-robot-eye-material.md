---
id: task-278-04-robot-eye-material
description: "A shared robot eye material M_RobotEye with its WPO in a material function shared with the chaser bodies, and four instances."
parent: task-278-chaser-eye-glow
kind: task
status: todo
---
# A shared robot eye material M_RobotEye with its WPO in a material function shared with the chaser bodies, and four instances

- goal: A shared robot eye material M_RobotEye with its WPO in a material function shared with the chaser bodies, and four instances.
- agent: bp
- depends_on: task-278-03-eye-slot-reimport
- inputs: chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), bp-chaser (vault/reference/tech/blueprints/enemies/bp-chaser.md), bp-shield-chaser (vault/reference/tech/blueprints/enemies/bp-shield-chaser.md), color-code (vault/reference/gdd/art-audio/color-code.md), blueprints-enemies-index (vault/reference/tech/blueprints/enemies/blueprints-enemies-index.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: First read the Custom node ChaserAnimWPO code of M_Chaser_Meshy_Anim and M_ShieldChaser_Anim and compare: the shield code with ClipOffset 0 must give exactly the chaser result for ClipId 0 and 1 (start rows 0, 16; frames 16, 15; RunRate loop on clip 0; clamp 14 on clip 1; Y mirror; PartId from TextureCoordinate 2). If not, stop and report (no asset change). Then create in /Game/RogueRigs/Materials/Robots/: MF_RobotAnimWPO (material function holding the shield Custom node code; inputs AnimTex (texture object), ClipOffset, RunRate; CPD 0 and 1, Time and UV2 read inside; output WPO). If a material function cannot be created through MCP or editor Python, stop and report: never copy the WPO code. M_RobotEye: lit, Roughness 0.6; parameters BaseColorTex (texture, default T_Chaser_Meshy_BaseColor), EyeColor (1, 0.05, 0.05), GlowAmount 23, UseEyeTexture (static switch, or scalar 0/1 fallback), Animated (static switch, or scalar weight fallback), AnimTex (default T_ChaserAnim), ClipOffset 0, RunRate 0.7; base colour = texture (UseEyeTexture) or EyeColor; emissive = EyeColor x GlowAmount x (texture red mask saturate((R - 2 max(G,B)) x 3) when UseEyeTexture, else 1); WPO = MF_RobotAnimWPO when Animated, else 0. Instances: MI_RobotEye_Chaser (texture on, animated, AnimTex T_ChaserAnim, ClipOffset 0), MI_RobotEye_ShieldChaser (texture on, animated, AnimTex T_ShieldChaserAnim, ClipOffset 0), MI_RobotEye_ShieldChaserPlain (same, ClipOffset 2), MI_RobotEye_GreyBox (texture off, not animated). New doc node bp-robot-eye-material (template blueprint.md) in tech/blueprints/enemies, linked from blueprints-enemies-index; it states the sync rule: RunRate and ClipOffset of an eye instance equal those of its body instance. Values provisional, owner 2026-10-21.
- done_when: WPO code comparison reported; MF_RobotAnimWPO, M_RobotEye and 4 instances exist, compile and are saved; every parameter read back on each instance; doc node created with revert (delete the new assets); lint clean; committed.
- editor_state: open (editor + MCP)
- token_ceiling: 150k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-278-chaser-eye-glow]] — parent global task.
