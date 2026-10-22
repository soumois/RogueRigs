---
id: task-282-03-texture-clean-shooter
description: "Clean the shooter textures (3d)."
parent: task-282-robot-texture-cleanup
kind: task
status: done
---
# Clean the shooter textures

- goal: Run Tools/art/blender/meshy_texture_clean.py for the shooter: first find whether the defect lies in the base colour or in the metallic and roughness maps, then repaint each panel class with the flat colour of the owner sheet (no defect image: compare the whole texture with the sheet and fix blotches, smears and colours that differ).
- agent: 3d
- depends_on: task-282-01-texture-clean-heavy
- inputs: art-3d-lessons (vault/reference/agents/art-3d-lessons.md), agent-3d (vault/reference/agents/agent-3d.md), robots-meshy-raw-2026-10-22 (vault/reference/tech/pipelines/robots-meshy-raw-2026-10-22.md), art-prompts-robots-meshy (vault/reference/gdd/art-audio/art-prompts-robots-meshy.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), color-code (vault/reference/gdd/art-audio/color-code.md), enemy-shooter (vault/reference/gdd/enemies/enemy-shooter.md); files: Art/Meshes/shooter-meshy/shooter_meshy_flat.fbx (task-281), Art/Source/shooter-meshy/meshy-raw/shooter_meshy_raw_basecolor.png, _metallic.png, _roughness.png, sheet vault/assets/shooter-turnaround.webp
- expected_output: Art/Meshes/shooter-meshy/shooter_meshy_clean_basecolor.png (2048 px) plus cleaned metallic and roughness maps only if they hold the defect, and Art/Meshes/shooter-meshy/shooter_meshy_texture_clean.report.json (cause found, texels changed per class, colours used)
- done_when: report names the cause; texels outside the treated classes unchanged; geometry and UV untouched; the Orchestrator renders the same camera (barrel_preview.py settings scaled to a 190 cm robot: front three-quarter and back three-quarter) before and after and sends both to the owner with the cleaned textures, plus a close-up of the defect area; Blender processes stopped
- editor_state: closed (Blender headless only)
- token_ceiling: 60000
- model: sonnet
- tokens: input 46, cache write 69684, cache read 1087619, output 6396
- runs: 1
- milestone: M2
- result: Cause: base colour (muddy shoulder logo, dark green stains in charcoal, olive baked shading on green); metallic/roughness clean. texture_clean/shooter.json; 521,064 texels changed (green (136,187,37), logo yellow/black), outside-class unchanged, heavy and kamikaze outputs byte-identical; renders Saved/Renders/task-282/shooter_* incl. logo close-up. Charcoal kept at raw (27,28,27); small orange patch on head back left.

## Relations
- [[task-282-robot-texture-cleanup]] — parent global task.
