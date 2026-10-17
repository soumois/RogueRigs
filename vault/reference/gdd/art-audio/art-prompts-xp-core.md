---
id: art-prompts-xp-core
description: "Four any-angle replacements for the flat big XP chip (data cube, nut cluster, gyro core, armoured sphere): prompt files, pros, cons, status."
parent: art-prompts
kind: note
---
# Big XP pickup — any-angle proposals

Owner request (2026-10-17): the big XP chip is a flat board with one interesting face (rest orientation YawOnly, [[bp-pickup-xp]]); its replacement must read the same whatever angle it is ejected and lands at, so it can rest in any orientation like the nut. Choice (owner 2026-10-17): A, the data cube.

## Proposals (prompt files in `Art/Source/xp-core/prompts/`)
| File | Proposal | For | Against |
|---|---|---|---|
| `a-data-cube.txt` | gunmetal bevelled cube, the same glowing window on all six faces | keeps the data-chip idea; simplest and cheapest; clearest when small | least original |
| `b-nut-cluster.txt` | six hex nuts welded around a glowing core | says "many XP nuts fused" | more triangles; busy when small |
| `c-gyro-core.txt` | glowing core inside three chunky rings at right angles | most distinctive | rings may vanish from above; more triangles |
| `d-armoured-sphere.txt` | plated ball with glowing seams | perfect any-angle read | close to the kamikaze (round body, glowing core) |

Recommendation given to the owner: a data cube.

## Chosen: data cube
- Owner image to 3D in Meshy (the owner ran it): `Art/Source/xp-core/meshy/` (FBX 5044 tris and base colour; raw renders in `inspect/`).
- Game mesh, validated by the owner 2026-10-17 (task-217): `Art/Meshes/xp-cube-big-mesh/SM_XPCubeBig.fbx`, body 748 tris (the chip it replaces is 844), 30 x 30 x 27.5 cm, centred pivot, slots body, caps, core_dark, core_bright (emissive set in Unreal) and glow_disc as the chip; built by `Tools/art/blender/xp_cube_big_mesh.py`, previews by `xp_cube_big_preview.py`.
- Done (task-221-01 to 03, 2026-10-17): Unreal import, swap of the chip for the cube in BP_PickupXPBig, rest orientation Full, materials; the chip assets, FBX, Blender scripts and `Art/Source/xp-chip/` were deleted. Glow hue still to check by the owner.

## Prompt use
Attach `vault/assets/xp-nut.png` (Image 1) and `vault/assets/xp-chip-big.png` (Image 2). Each prompt asks for four orientations plus the nut for scale. The prompts ask for one magenta-pink hue for core and glow, proposed to close the lilac versus magenta mismatch in [[color-code]] (owner decision open).

## Relations
- [[art-prompts]] — parent: method and locked phrases.
- [[bp-pickup-xp]] — the current chip, its scale and rest orientation.
- [[color-code]] — XP colours and the open hue mismatch.
