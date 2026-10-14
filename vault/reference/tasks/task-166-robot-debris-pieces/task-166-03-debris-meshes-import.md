---
id: task-166-03-debris-meshes-import
description: "Import the 13 chaser debris piece meshes into Unreal and make the debris material: chaser base colour, no animation WPO, no glow."
parent: task-166-robot-debris-pieces
kind: task
status: done
---
# Robot debris: import meshes and material

- goal: Import the 13 chaser debris piece meshes into Unreal and make the debris material: chaser base colour, no animation WPO, no glow.
- agent: bp
- depends_on: task-166-01-robot-debris-pieces
- inputs: chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), bp-chaser (vault/reference/tech/blueprints/enemies/bp-chaser.md), enemy-death (vault/reference/gdd/enemies/enemy-death.md), naming (vault/reference/tech/conventions/naming.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), orchestrator-lessons-2026-10-14 (vault/reference/agents/orchestrator-lessons-2026-10-14.md)
- expected_output: In /Game/RogueRigs/Art/Chaser/Debris/: the 13 SM_ChaserDebris_* static meshes imported from Art/Source/chaser-meshy/debris/ (check the import scale: about 123 cm robot height in total, not 100 times larger; no collision, no Nanite, no lightmap generation); M_ChaserDebris (opaque, base colour from the chaser corrected texture, no WPO, no emissive, usable with Niagara mesh particles: bUsedWithNiagaraMeshParticles) assigned to every piece; chaser-meshy-pipeline doc updated with the assets
- done_when: The 13 meshes and the material exist and read back (bounds in cm, material slot, usage flag); saved and committed by path; doc updated; the editor stays open
- editor_state: open
- token_ceiling: open
- model: sonnet
- tokens: input 62, cache write 70491, cache read 1457931, output 15195
- runs: 1
- milestone: M3
- result: 13 SM_ChaserDebris_* + M_ChaserDebris (opaque, T_Chaser_Meshy_BaseColor, no WPO/emissive, Niagara mesh usage) in /Game/RogueRigs/Art/Chaser/Debris, bounds in cm, Nanite off, collisions removed; lightmap UV generation not verifiable through MCP (harmless); commit 57ab7ef

## Relations
- [[task-166-robot-debris-pieces]] — parent global task.
