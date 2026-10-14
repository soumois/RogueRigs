---
id: task-166-03-debris-meshes-import
description: "Import the 13 chaser debris piece meshes into Unreal and make the debris material: chaser base colour, no animation WPO, no glow."
parent: task-166-robot-debris-pieces
kind: task
status: todo
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
- model: open
- tokens: open
- runs: open
- milestone: M3
- result: open

## Relations
- [[task-166-robot-debris-pieces]] — parent global task.
