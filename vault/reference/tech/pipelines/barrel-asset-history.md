---
id: barrel-asset-history
description: "Task map of the barrel work (tasks 101 to 108), what later steps superseded, Kie credits spent. Open if a task node disagrees with the asset."
parent: pipelines-index
kind: note
---
# Barrel asset history

The final state is in [[barrel-asset]]. Task nodes keep their original text, so some are superseded; this map says which.

## Task map
- task-101 (steps 01 to 13): the asset. 01 source image (2 candidates, shape 2 kept); 02 sticker, then moved onto one facet; 03 warning sign on the image; 04 first Blender mesh; 05 UVs, lightmap channel, mask contract, preview script; 06 edge wear by vertex colour; 07 thin edge bands, bolts, stepped cap; 08 own explosion sign replacing the flame; 09 logo moved to the lid; 10 sign doubled; 11 lid logo 1.3 times larger; 12 sign 1.6 times larger again; 13 sign raised so the burst centre sits mid-height between the rings.
- task-102: the company logo, 4 variations, variation 1 kept ([[kessler-voss-logo]]).
- task-106: the shared wear mask `T_WearMask.png` (01 blobs, 02 scratch lines only: the final one).
- task-107: integration; 01 cpp fits the barrel to one cell, 02 bp imports assets and sets `BP_ArenaGrid`.
- task-108: the barrel stood below the floor; now at z 0 with its slab kept.

## Superseded
- Mask recipe in task-101-05 (`mix(bare metal #8A9099, paint, mask)`) and task-106-01 (blob mask): replaced by the scratch lerp in task-101-06 and task-101-07 ([[blender-asset-route]]).
- Sign size: 26 cm in task-101-08, 52 cm in 10, 83 cm in 12 (final).
- task-101-03 asked for a sign with no yellow and black; the owner then chose a yellow and black triangle (2026-10-11); [[color-code]] notes the exception.
- The global task-101 result says the Unreal import was not done: task-107 did it.
- Task 106 acceptance says "patchy look": the final mask is scratch lines only.

## Kie credits (2026-10-11), about 6 per image
Barrel 48: first candidates 12, sticker 12, sticker moved to one facet 12, warning sign 12. Logo 24: four variations. Total 72. The Blender steps spent none.

## Relations
- [[pipelines-index]] — parent index.
- [[barrel-asset]] — the final asset.
- [[kie-image-workflow]] — how images are generated and what they cost.
- [[kessler-voss-logo]] — the logo task.
