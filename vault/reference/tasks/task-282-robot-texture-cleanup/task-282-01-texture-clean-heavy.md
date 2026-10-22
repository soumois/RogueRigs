---
id: task-282-01-texture-clean-heavy
description: "Clean the heavy textures (3d)."
parent: task-282-robot-texture-cleanup
kind: task
status: todo
---
# Clean the heavy textures

- goal: Write Tools/art/blender/meshy_texture_clean.py (generic, one config per robot in Tools/art/blender/texture_clean/<robot>.json, on the model of meshy_texture_fix.py) and run it for the heavy: first find whether the defect lies in the base colour or in the metallic and roughness maps, then repaint each panel class with the flat colour of the owner sheet (image vault/assets/heavy-meshy-defect-back.webp: arm, lower-leg and back panels look translucent and glass-like with grey camouflage blotches where the sheet shows flat opaque blue panels).
- agent: 3d
- depends_on: task-281-04-flatten-chaser-v2
- inputs: art-3d-lessons (vault/reference/agents/art-3d-lessons.md), agent-3d (vault/reference/agents/agent-3d.md), robots-meshy-raw-2026-10-22 (vault/reference/tech/pipelines/robots-meshy-raw-2026-10-22.md), art-prompts-robots-meshy (vault/reference/gdd/art-audio/art-prompts-robots-meshy.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), color-code (vault/reference/gdd/art-audio/color-code.md), enemy-heavy (vault/reference/gdd/enemies/enemy-heavy.md); files: Art/Meshes/heavy-meshy/heavy_meshy_flat.fbx (task-281), Art/Source/heavy-meshy/meshy-raw/heavy_meshy_raw_basecolor.png, _metallic.png, _roughness.png, sheet vault/assets/heavy-turnaround.webp
- expected_output: Art/Meshes/heavy-meshy/heavy_meshy_clean_basecolor.png (2048 px) plus cleaned metallic and roughness maps only if they hold the defect, and Art/Meshes/heavy-meshy/heavy_meshy_texture_clean.report.json (cause found, texels changed per class, colours used)
- done_when: report names the cause; texels outside the treated classes unchanged; geometry and UV untouched; the Orchestrator renders the same camera (barrel_preview.py settings scaled to a 190 cm robot: front three-quarter and back three-quarter) before and after and sends both to the owner with the cleaned textures, plus a close-up of the defect area; Blender processes stopped
- editor_state: closed (Blender headless only)
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-282-robot-texture-cleanup]] — parent global task.
