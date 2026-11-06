---
id: bp-blob-shadow-material
description: "M_BlobShadow and MI_BlobShadow (task-264, task-472): unlit translucent purple-grey soft disc under actors; parameters, per-actor opacity, rebuild script."
parent: blueprints-fx-index
kind: doc
---
# M_BlobShadow and MI_BlobShadow

- asset_path: /Game/RogueRigs/FX/BlobShadow/M_BlobShadow (Material) and /Game/RogueRigs/FX/BlobShadow/MI_BlobShadow (MaterialInstanceConstant, no override). The C++ references the instance.
- purpose: the light-independent actor shadow of [[visual-style]] (owner 2026-11-05): dark desaturated purple-grey, never black, slightly soft edge, readable silhouette.
- settings: Unlit, Translucent, two sided, depth test on (hidden by walls and props), no lighting: no light changes it.
- graph (task-472-02): `Tint` (vector) into EmissiveColor; custom node `BlobShadowMask(UV, Edge, Op)` = `(1 - smoothstep(Edge, 1, 2 x distance(UV, 0.5))) x saturate(Op)` into Opacity: full alpha up to `EdgeStart` of the radius, soft edge over the outer part. The task-264 falloff nodes (Distance, Power, Softness renamed `EdgeStart`) remain unconnected: deleting expressions asserts in headless 5.8.
- parameters and defaults (owner 2026-11-06 Q3, provisional): `Opacity` 0.6, flagged custom primitive data index 0 (each actor writes its own value, [[sys-blob-shadow]]); `EdgeStart` 0.8 (soft outer 20 percent of the radius); `Tint` linear (0.0092, 0.0065, 0.0102), aimed at sRGB (45, 35, 55) on screen: the plain conversion of (45, 35, 55) showed about (80, 60, 110) after the fixed exposure and grading.
- usage: engine Plane (100 x 100 uu) a few uu above the floor, scaled by `URRBlobShadowComponent`.
- rebuild: `Tools/unreal/blob_shadow_material.py` (headless `-ExecutePythonScript`, editor closed, copy to a path without spaces; idempotent; lines `BLOBSHADOW`, also to the file named by env `BLOBSHADOW_OUT`) ([[tool-unreal-reimport]]).
- verified (task-472-02, commit 0028c7a2): read back Unlit, Translucent, two sided, emissive from Tint, opacity from the mask node, Opacity on custom primitive data 0; -game frames (Saved/Captures/task472-shadows-b, task472-shadows-barrel, task472-capture-move): purple-grey soft discs under the miner, drone, turrets and robots, in and out of the miner light pool; unblended about sRGB (35, 30, 43).
- unverified: chest and barrel discs show only a thin rim at SizeScale 1.2 (owner to judge).
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[sys-blob-shadow]] — the C++ component that draws it and writes its opacity.
- [[visual-style]] — the actor shadow rule it implements.
- [[lights-no-shadows]] — why the first fake shadow existed.
- [[readability-rules]] — the disc must not hide the silhouettes or the telegraphs.
- [[perf-robot-components-audit]] — the cost budget of per-robot components.
- [[tool-unreal-reimport]] — lists the rebuild script.
