---
id: task-216-shield-texture
description: "Texture the raw Meshy riot shield of the shield chaser in headless Blender from its four validated views; emblem decal comes later."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Shield chaser riot shield: Blender texture

- request: "Oui, fais la texture du bouclier maintenant sous Blender, l'emblème viendra ensuite en décalque." (owner 2026-10-17)
- goal: an export-ready FBX of the riot shield with the look of the validated views: flat yellow front, yellow and black diagonal stripes on the two cut corners and the top edge, dark gunmetal back, grip, bracket and bolts; the front centre left plain for the later emblem decal.
- acceptance: FBX and report in `Art/Meshes/ChaserShield/`; renders matching the four views criterion by criterion ([[mesh-fidelity-loop]]); renders sent to the owner.
- gdd_check: [[art-prompts-shield]], [[color-code]], [[art-style-guide]] A1, A5, [[enemy-chaser]]. Conflicts: stripes are reserved for the heavy, accepted as a detail on the shield (owner 2026-10-17). Open: triangle budget of the shield (raw Meshy mesh 932 tris; chaser 3115 + shield is above the 3000 chaser budget), front-only blocking; emblem file not yet generated. Height 82 cm = two thirds of the chaser height, as in the validated sheet prompt (provisional).
- confirmed_by_owner: yes
- milestone: M1
- next_step: task-216-02-shield-v2-simplify
- result: open

## Subtasks
- [[task-216-01-shield-blender-texture]] — 3D agent: materials and stripe texture on the Meshy mesh, scale, pivot, export, fidelity renders.
- [[task-216-02-shield-v2-simplify]] — 3D agent: second Meshy mesh simplified to about 1k flat tris, side bracket removed, texture rebaked, stripes over the top.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[art-prompts-shield]] — images, prompts and raw mesh of the shield.
