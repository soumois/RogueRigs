---
id: barrel-asset
description: "The explosive barrel as built and integrated (tasks 101 to 108): files, slots, decals, wear, rejected approaches, Unreal side. Open before touching it."
parent: pipelines-index
kind: doc
---
# Barrel asset

Validated by the owner and seen in game, 2026-10-12. Spec: [[barrel]]. Built with [[blender-asset-route]].

## What it is
- `Art/Meshes/barrel/SM_Barrel.fbx` (498 triangles, budget 600), about 90 x 90 x 134 cm (decal offset adds 0.27 cm), bottom-centre pivot, 8 flat sides, two rings, grey lid with a stepped cap and 4 small bolts.
- 9 material slots in this order: body, ring, lid, cap, cap edge band, lid edge band, ring edge band, sticker, warning. Slot 0 is the body because the grid overrides slot 0 with `BarrelMaterial`.
- Decals: the [[kessler-voss-logo]] plate (about 36 x 15 cm) flat on the lid top; an explosion warning triangle (about 83 cm wide, wrapping two facets and both rings, burst centre at mid-height between the rings). The sign is our own drawing (`T_Barrel_Warning.png`, own burst shape, no text), made after a third-party stock sign shown by the owner; yellow and black although [[color-code]] keeps hazard stripes for the heavy (owner accepted).
- Wear: thin lighter edge bands with seeded gaps; faint scratches on lid and cap only (`T_WearMask.png`).

## Rejected approaches (do not retry)
Uniform grey rim; geometric light-metal chips; tileable blob mask (cow spots); vertex-colour edge chips. The reference image shows only thin lighter edges, no patches.

## Unreal
- Assets in `/Game/RogueRigs/Art/Barrel/`: `SM_Barrel`, three textures, `M_BarrelPaint` with instances `MI_Barrel_*`, sticker and warning materials (warning Masked).
- No barrel Blueprint exists: barrels are a grid cell type drawn by `BP_ArenaGrid` (`BarrelMesh` = SM_Barrel, `BarrelMaterial` = `MI_Barrel_body`) ([[bp-arena-grid]]).
- C++ ([[sys-grid]]): barrels render with a uniform scale from the mesh bounds (fits one cell and `BlockHeight`), bottom at z 0; `BarrelCollisionMesh` keeps the collision on the cylinder; barrel cells keep a floor slab (`HasFloorSlab`), so a destroyed barrel adds no second floor.

## Open
An unused third UV channel from the importer; two MapCheck usage-flag warnings on the sticker and warning materials, not re-run. `vault/assets/barrel.png` is the validated source image but no longer matches the mesh.

## Relations
- [[pipelines-index]] — parent index.
- [[barrel]] — the gameplay spec.
- [[blender-asset-route]] — how it was built.
- [[kessler-voss-logo]] — the sticker.
- [[bp-arena-grid]] — where the mesh is assigned.
- [[sys-grid]] — the render transform and floor slab rules.
