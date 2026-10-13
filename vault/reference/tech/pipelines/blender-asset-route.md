---
id: blender-asset-route
description: "Blender route (no Meshy) for simple props: scripts, headless command, UV channels, mask contract, preview, FBX quirks. Open before a Blender mesh task."
parent: pipelines-index
kind: doc
---
# Blender asset route

Used when a 3D subtask says "Blender route" ([[agent-3d]]): no Meshy call, no credit, no API key. First asset: the barrel ([[barrel-asset]]). Owner decision 2026-10-11: Meshy is not the default for simple props.

## Running
- Headless: `"$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/<script>.py -- <args>`. `BLENDER_EXE` is a Windows user variable (Blender 5.2). Python exists only inside Blender, numpy included.
- Scripts in `Tools/art/blender/`: `barrel.py` (mesh, FBX, report), `warning_sign.py` (sign texture), `wear_mask.py` (scratch mask, seed 108, deterministic), `barrel_preview.py <fbx> <out.png>` (render), `cleanup.py` (Meshy meshes only).
- Output: `Art/Meshes/<asset>/SM_<Name>.fbx` in centimetres, bottom-centre pivot, plus `SM_<Name>.report.json` (`tris_out`, `dimensions_cm`, `materials`, `uv_layers`, `uv_checks`, `mask_contract`, `warnings`).

## Conventions (art charter level 1, [[art-style-guide]] A1)
- Flat shading, flat colour per material ([[color-code]]); no per-asset painted texture.
- Wear: thin lighter bands on edges (geometry with their own flat materials) and faint scratch lines from the shared mask `Art/Textures/T_WearMask.png` (grayscale, tileable, white = clean).
- Stickers and signs: decals as copies of the covered faces offset 0.25 cm, with ONE planar UV projection so a decal can wrap over facets; textures with alpha need Masked in Unreal.
- UV channels: `UVMap` (UV0: decals 0 to 1, body 100 cm per UV unit for the mask) and `LightMap` (UV1: every triangle, no overlap, margin at least 2 px at 256, bounds inside 0 to 1). The report checks both numerically.
- `mask_contract` in the report is the recipe for the Unreal master material: colour = lerp(paint, lighter tone, (1 - mask) x strength), mask on UV0, wrap.

## Checking without a screen
Agents never open a viewport. They check by numbers. The Orchestrator renders `barrel_preview.py` (1000 x 1000 PNG, EEVEE, orthographic scale 200, azimuth -35, elevation 28, distance 400 cm, target z 67, one sun energy 4.0 rotated (50, 10, -60), white world 0.35, Standard) and sends the PNG to the owner with `SendUserFile`.

## Quirks
- Vertex colours survive FBX export only with `colors_type="LINEAR"` (unused in the final barrel).
- Unreal import flips the sign of Y in the bounds; sizes are unchanged. The importer adds an unused third UV channel (auto lightmap) unless disabled in the editor: the MCP import tool has no such option.
- The FBX references textures by relative path; Unreal needs them imported separately.
- Git: only the script, the FBX, the report and the textures are committed.
- After a re-export of an FBX already in Unreal, reimport it in place with [[tool-unreal-reimport]] (editor closed).

## Relations
- [[pipelines-index]] — parent index.
- [[asset-pipeline]] — the overall pipeline and mesh values.
- [[agent-3d]] — the agent that runs this route.
- [[barrel-asset]] — the first asset built this way.
- [[art-style-guide]] — A1, the texture charter.
- [[tool-unreal-reimport]] — reimport a mesh in place after a re-export.
- [[mesh-fidelity-loop]] — how a mesh is judged against its image; the subtask lists criteria and a max number of returns.
