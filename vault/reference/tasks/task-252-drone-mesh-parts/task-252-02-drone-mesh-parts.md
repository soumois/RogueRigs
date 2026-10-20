---
id: task-252-02-drone-mesh-parts
description: "Model the parabolic dish with its stem as a separate part, in the look of the validated image."
parent: task-252-drone-mesh-parts
kind: task
status: done
---
# Drone parabolic dish

- goal: Model the parabolic dish with its stem as a separate part, in the look of the validated image.
- agent: 3d
- depends_on: task-252-01-drone-mesh-parts
- inputs: drone (vault/reference/gdd/player/drone.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), color-code (vault/reference/gdd/art-audio/color-code.md)
- expected_output: DroneAntenna FBX (Blender script in Tools/art/blender/, off-white dish, dark stem with rust, at most 500 triangles, pivot at the foot of the stem, Z up axis of spin, same scale and units as DroneBase) and a report.
- done_when: FBX exists, at most 500 triangles, FBX_SCALE_UNITS, pivot and spin axis checked by numbers, shown beside the base in a render sent to the owner, Blender processes stopped.
- editor_state: not needed (Blender headless)
- token_ceiling: 150k
- model: sonnet
- tokens: input 18, cache write 58100, cache read 380885, output 15808
- runs: 1
- milestone: M1
- result: SM_DroneAntenna modelled in Blender (drone_antenna.py): 392 tris, 20 cm dish tilted 30 deg on a bent dark mount with a feed rod, pivot at the flange foot on the Z spin axis, attached at (-4.257, 0.837, 28.752) (gap 0.07 cm); flat colours as vertex colours (off-white, gunmetal, rust), material meshy_drone_antenna; SM_Drone_all.fbx for viewing. Checks by numbers pass (0 open edges, UV ok). Renders sent to the owner; Workbench preview shows one tone only, vertex colours not seen rendered.

## Relations
- [[task-252-drone-mesh-parts]] — parent global task.
