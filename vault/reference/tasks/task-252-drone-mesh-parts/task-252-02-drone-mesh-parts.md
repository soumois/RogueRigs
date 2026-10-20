---
id: task-252-02-drone-mesh-parts
description: "Model the parabolic dish with its stem as a separate part, in the look of the validated image."
parent: task-252-drone-mesh-parts
kind: task
status: todo
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
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-252-drone-mesh-parts]] — parent global task.
