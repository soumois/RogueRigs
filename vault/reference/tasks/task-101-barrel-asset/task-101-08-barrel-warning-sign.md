---
id: task-101-08-barrel-warning-sign
description: "The flame pictogram is replaced by an explosion warning triangle that can wrap over several facets (owner 2026-10-11)."
parent: task-101-barrel-asset
kind: task
status: done
---
# The flame pictogram is replaced by an explosion warning tria

- goal: The flame pictogram is replaced by an explosion warning triangle that can wrap over several facets (owner 2026-10-11).
- agent: 3d
- depends_on: task-101-07-barrel-edge-highlights
- inputs: agent-3d (vault/reference/agents/agent-3d.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md); files Tools/art/blender/barrel.py, Tools/art/blender/barrel_preview.py, Art/Meshes/barrel/SM_Barrel.report.json
- expected_output: Owner reference: a yellow warning triangle with a thick black border and a black explosion burst, seen in the image the owner sent (a third-party stock sign by Clarion, with its name and URL printed at the bottom): use it as a visual reference ONLY. Draw our own version, do not copy or trace the image, never reproduce the maker name, URL or any text. Blender route, no credit. (1) A script Tools/art/blender/warning_sign.py (bpy + numpy, deterministic) writes Art/Meshes/barrel/T_Barrel_Warning.png, 512 x 512 RGBA, transparent background: rounded yellow triangle (about #FFD500, close to the company yellow), thick black rounded border, black spiky explosion burst (own shape: 7 to 9 irregular spikes of different heights from a jagged base, not a copy of the reference), hard edges. (2) In barrel.py replace the flame pictogram decal (T_Barrel_Pictogram.png, M_Barrel_pictogram) by this sign: M_Barrel_warning; the sign may wrap over several body facets: build the decal as offset copies (about 0.25 cm) of every body face it covers, with ONE planar UV projection of the sign over them (so it continues across the facet edges), not as one quad; size about 26 cm wide, centred on the vertical corner next to the logo-free front, between the rings. Remove T_Barrel_Pictogram.png from the export. (3) Keep UVMap and LightMap rechecked numerically (no overlap, margin >= 2 px at 256 px, bounds 0..1; the new decal faces get lightmap islands too), dimensions 90 x 90 x 134 cm, pivot, 600 triangles max. (4) Update barrel_preview.py (new material, alpha blend) and SM_Barrel.report.json (materials, decal faces, warnings: alpha blend must be Masked or Translucent in Unreal); run the preview once to the scratchpad without opening it. Commit by name (script, texture, FBX, report).
- done_when: T_Barrel_Warning.png exists (RGBA, transparent background, no text); the flame decal and its texture are gone; the sign decal spans two or more facets with one UV projection; tris under 600; dimensions and pivot unchanged; UV and lightmap checks pass; barrel_preview.py runs; committed by name.
- editor_state: any
- token_ceiling: 35000
- model: open
- tokens: open
- runs: 1
- milestone: M2
- result: Explosion triangle sign (own burst shape, no text), decal wrapped over two facets with one UV projection, recentred on the visible corner; 430 tris. Commits ab94f39, 7c3dbfe. Cost counted with task-101-06 (same resumed agent).

## Relations
- [[task-101-barrel-asset]] — parent global task.
