---
id: task-301-03-console-c2-retouch
description: "Repaint the console C2 dish back and foot lamps (3d): console c2 uplink mast mesh, step 03."
parent: task-301-console-c2-mesh
kind: task
status: review
---
# Repaint the console C2 dish back and foot lamps

- goal: In Blender: repaint the dish-back area of the base colour texture after the back view of the owner sheet; turn the four foot lamps into clean round amber caps (image 3) assigned to the OtherOrange slot, flat emissive look; re-export both FBX and the texture; renders of the dish back and the top view compared with images 2 and 3. The Orchestrator shows the render to the owner before task-301-04.
- agent: 3d
- depends_on: task-301-02-console-c2-cut
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), mesh-fidelity-loop (vault/reference/tech/pipelines/mesh-fidelity-loop.md); files: vault/assets/console-c2-turnaround.webp (back view); Art/Source/console-meshy/owner/ref2-meshy-top-feet-lights-current.png; Art/Source/console-meshy/owner/ref3-meshy-top-feet-lights-wanted.webp; Art/Source/console-meshy/clean/
- expected_output: updated console_c2_base.fbx, console_c2_head.fbx, repainted base colour PNG, renders, report
- done_when: dish back and foot caps visible in the renders; caps in OtherOrange; owner shown the renders and gave his go; no Blender process left; committed
- editor_state: any
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: Renders made (commit 1fcf8ca2): dish back repainted after the sheet back view, 8 round amber foot caps (2 per foot) in OtherOrange; waiting for the owner go on the before/after renders before task-301-04.

## Relations
- [[task-301-console-c2-mesh]] — parent global task.
