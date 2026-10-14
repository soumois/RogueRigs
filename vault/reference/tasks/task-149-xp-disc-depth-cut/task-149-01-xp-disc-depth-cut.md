---
id: task-149-01-xp-disc-depth-cut
description: "Remove the flat cut at the bottom of the glow disc by pushing the camera-facing disc toward the camera."
parent: task-149-xp-disc-depth-cut
kind: task
status: done
---
# XP glow disc: flat bottom cut

- goal: Remove the flat cut at the bottom of the glow disc by pushing the camera-facing disc toward the camera.
- agent: bp
- depends_on: none
- inputs: bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), color-code (vault/reference/gdd/art-audio/color-code.md)
- expected_output: In /Game/RogueRigs/Art/XPNut/M_XPGlowDisc add a scalar parameter DepthPush (cm, default 60) and add CameraVectorWS (normalised, from the vertex toward the camera) x DepthPush to the World Position Offset (the existing WPO graph stays: this is one more Add before the output), so the disc sits in front of the floor along the line of sight without moving on screen; MI_XPGlowDisc_Nut DepthPush 60 (Radius 50) and MI_XPGlowDisc_Chip DepthPush 90 (Radius 80); everything else (Radius, Color, Intensity, Falloff, Fade) unchanged; recompile with no error; read back; save the material and the two instances only. If CameraVectorWS is not available as an expression class, use the equivalent (a Camera Vector node, ViewProperty or a Transform of (0, 0, 1) from View to World negated: the direction from the scene toward the camera) and say what you used. Doc: bp-pickup-xp (DepthPush, why, provisional values 60 and 90, how to tune: if the bottom is still cut raise DepthPush, if the glow shows through walls lower it). STRICT: save only the named assets with save_assets; if a save fails, a property cannot be set or a call is denied, STOP and report; no SlateInspectorToolset, no key presses (never Ctrl+S), no UI automation; the owner's Content/__ExternalActors__ files are not yours (never touch or commit them); no screenshot tool; no performance test; update the doc node bp-pickup-xp; commit by name.
- done_when: DepthPush reads 60 and 90 on the instances; the material compiles; assets saved and modified in git; doc updated
- editor_state: open
- token_ceiling: open
- model: sonnet
- tokens: input 44, cache write 39252, cache read 498161, output 610
- runs: 1
- milestone: M2
- result: DepthPush parameter (60 nut, 90 chip) added to M_XPGlowDisc: WPO = billboard + normalised (camera position - object pivot) x DepthPush; compiles (CameraVector node not allowed in the vertex shader); visual result for the owner.

## Relations
- [[task-149-xp-disc-depth-cut]] — parent global task.
