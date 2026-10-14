---
id: task-147-01-xp-disc-billboard
description: "Make the glow disc always face the camera and stay centred on the pickup, in the material."
parent: task-147-xp-disc-billboard
kind: task
status: in-progress
---
# XP glow disc: camera-facing and centred

- goal: Make the glow disc always face the camera and stay centred on the pickup, in the material.
- agent: bp
- depends_on: none
- inputs: bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), color-code (vault/reference/gdd/art-audio/color-code.md)
- expected_output: In /Game/RogueRigs/Art/XPNut/M_XPGlowDisc add a World Position Offset that turns the disc into a camera-facing quad centred on the pickup: the disc mesh carries a radial UV0 (u and v in 0 to 1, 0.5 is the centre), so for each vertex WPO = (ObjectPositionWS + CameraRightWS x ((u - 0.5) x 2 x Radius) + CameraUpWS x ((v - 0.5) x 2 x Radius)) - AbsoluteWorldPosition, with CameraRightWS and CameraUpWS from Transform nodes (View to World) of (1, 0, 0) and (0, 1, 0), and a new scalar parameter Radius (world cm; default 50). The glow formula stays: distance from the UV centre. Set Radius 50 on MI_XPGlowDisc_Nut and 80 on MI_XPGlowDisc_Chip (the chip actor has scale 1.8 and the disc mesh is built for the world radius, so use world units: ObjectPositionWS is the pivot at the pickup centre, not the floor). Recompile; check the material compiles with no error and the two instances read back; note that the mesh bounds already include the old floor disc, so culling is safe. Doc: bp-pickup-xp (the disc is now camera-facing, Radius is a parameter, how to change the size). STRICT: save only the named assets with save_assets; if a save fails, a property cannot be set or a call is denied, STOP and report; no SlateInspectorToolset, no key presses (never Ctrl+S), no UI automation; the owner's Content/__ExternalActors__ files are not yours (never touch or commit them); no screenshot tool; no performance test; update the doc node bp-pickup-xp; commit by name.
- done_when: The material compiles (recompile returns without error), Radius reads 50 and 80 on the two instances, the old values (Color, Intensity, Falloff) unchanged; assets saved; doc updated
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-147-xp-disc-billboard]] — parent global task.
