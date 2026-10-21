---
id: task-261-01-no-shadows
description: "No object casts a shadow for now: shadow casting is switched off on the lights of the arena level."
parent: task-261-no-shadows
kind: task
status: done
---
# No shadows

- goal: No object casts a shadow for now: shadow casting is switched off on the lights of the arena level.
- agent: bp
- depends_on: none
- inputs: art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), visual-style (vault/reference/gdd/art-audio/visual-style.md), ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: On L_ArenaM1 (and any other level of the run) every light that casts shadows (directional light first) gets CastShadows false, read back, saved; Lumen GI and everything else unchanged; a vault node (provisional, owner 2026-10-20, temporary) lists the lights changed and how to revert; a provisional line under A12 in art-style-guide.
- done_when: Shadow casting reads false on every light of the arena level after save; the node lists them with the revert steps; lint clean.
- editor_state: open (editor + MCP)
- token_ceiling: 100k
- model: sonnet
- tokens: input 128, cache write 124703, cache read 5664424, output 1419
- runs: 1
- milestone: M1
- result: Shadow casting switched off on the only two lights of L_ArenaM1: DirectionalLight_0 and SkyLight_0 (castShadows false, other flags kept; BP_Kamikaze CoreLight was already off); Lumen, materials and per-object flags untouched; node lights-no-shadows with revert steps; A12 and visual-style got a provisional line. Read back false after save. Not seen on screen. Lumen and virtual shadow maps cannot be switched off from here; sky light flag is the first to re-tick if the lighting looks wrong.

## Relations
- [[task-261-no-shadows]] — parent global task.
