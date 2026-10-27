---
id: asteroid-asset
description: "Background asteroid meshes (Blender route): files, budget, seeds, build recipe, rebuild command, preview camera. Open to rebuild or re-import SM_Asteroid_01 to 04."
parent: pipelines-index
kind: doc
---
# Asteroid asset (task-334-01)

Four low-poly faceted rocks for the void around the arena ([[task-334-near-asteroids]]), built by `Tools/art/blender/asteroid.py` on the Blender route ([[blender-asset-route]]): no Meshy, no credit, no source image.

## Files (`Art/Meshes/Asteroid/`)
- `SM_Asteroid_01.fbx` to `SM_Asteroid_04.fbx`, each with `.report.json` (tris, dimensions, islands, materials, UVs, warnings); `preview.png` (4 variants, 1600 x 600).
- FBX in centimetres (`FBX_SCALE_UNITS`, [[art-3d-lessons]]).

## Values
- Budget: at most 400 triangles; built: 320 each, one island.
- Size: largest bbox dimension 100 cm before scaling; pivot at the bbox centre.
- Seeds: 3401, 3402, 3403, 3404 (variants 01 to 04). Deterministic, no other randomness.
- One material slot `M_Asteroid`, flat colour violet-grey (linear 0.16, 0.135, 0.21), darker than the arena rocks ([[color-code]], "Background asteroids"). No texture; one UV channel (`UVMap`, smart project), no lightmap channel.

## Recipe
Icosphere (bmesh subdivisions 3, 320 triangles), radial displacement from 5 random lobes plus per-vertex jitter of 0.12, non-uniform stretch, then bbox-centred and scaled to 1 m. Flat shading, facets visible ([[art-style-guide]] A1).

## Rebuild
`"$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/asteroid.py -- --outdir Art/Meshes/Asteroid` writes the FBX, reports and preview. Stop the Blender process afterwards. After a re-export of an imported mesh, use [[tool-unreal-reimport]].

## Preview
Orthographic scale 5.6 m, camera pitch -50 degrees (rotation X 40), one sun energy 3.0 rotated (50, 10, -60), uniform grey world, Standard view transform, EEVEE.

## Relations
- [[pipelines-index]] — parent index.
- [[blender-asset-route]] — the route and headless command.
- [[task-334-near-asteroids]] — the global task these meshes serve.
- [[color-code]] — background asteroid colour.
