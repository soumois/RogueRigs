---
id: task-220-06-shield-chaser-reimport
description: "cpp: headless reimport of the rebaked shield chaser mesh and anim texture, mesh build settings, UV channel 2 RENDER_DATA check."
parent: task-220-shield-chaser-integration
kind: task
status: in-progress
---
# Shield chaser: headless reimport and channel 2 check

- goal: `SM_ShieldChaser_Meshy_Anim` and `T_ShieldChaserAnim` in Unreal match the files rebaked by task-220-05, with the build settings MCP could not set, and the part index in UV channel 2 proven in the render data.
- agent: agent-cpp (headless UnrealEditor-Cmd, no C++ change, no build)
- depends_on: task-220-05-shield-pavise-pose
- inputs: `vault/reference/tech/pipelines/tool-unreal-reimport.md` (command, `convert_scene_unit` pin, restore unchanged uassets), `vault/reference/tech/pipelines/chaser-meshy-pipeline.md` (Unreal section: build settings, channel 2 check of task-163-01), `vault/reference/agents/art-3d-lessons.md` (engine and import traps), `vault/reference/agents/agent-cpp.md`; files `Art/Meshes/shield-chaser/SM_ShieldChaser_Meshy_Anim.fbx`, `T_ShieldChaserAnim.exr` and the report; assets in `/Game/RogueRigs/Art/ShieldChaser/` (task-220-02 result: LightMapCoordinateIndex 1, bounds extension 60, no collision, Nanite off, slots set to MI_ShieldChaser_Body, _Shield, _Emblem).
- method: editor closed (the Orchestrator closed it). 1) Reimport the mesh with `Tools/unreal/reimport_static_mesh.py`; check bounds (z 0 to 123, bash reach x about 57) and that the three slot materials are still the instances. 2) Reimport `T_ShieldChaserAnim` from the EXR in place (a small headless Python script under `Tools/unreal/` if none exists), keeping TC_HDR, no mips, Nearest, sRGB off, never stream, Clamp; read back the size 20 x 62. 3) Mesh build settings headless (`UnrealEditor-Cmd ... -ExecutePythonScript=<file>`, not `-run=pythonscript`, per the lessons): Generate Lightmap UVs off, LightMapCoordinateIndex 1, full precision UVs, then save. 4) Read RENDER_DATA with Geometry Script (`copy_mesh_from_static_mesh`, requested LOD render data) and check UV channel 2 on every triangle corner: shield corners 0.45, chaser corners as in SM_Chaser_Meshy_Anim. 5) Restore any `.uasset` that the tools saved without change.
- done_when: log lines of each step; channel 2 check passes; `git status` shows only the intended assets changed; commit by path `task-220-06-shield-chaser-reimport: <summary>`.
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-220-shield-chaser-integration]] — parent global task.
