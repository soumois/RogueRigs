---
id: orchestrator-lessons-2026-10-14
description: "Engine, tool and process lessons of the 2026-10-14 Meshy chaser session: FBX scale, lightmap UV trap, MCP, permissions, git. Open before Unreal work."
parent: agents-index
kind: note
---
# Orchestrator lessons, session of 2026-10-14 (Meshy chaser)

Complements [[orchestrator-lessons-2026-10-12]] and [[art-3d-lessons]]. Pipeline: [[chaser-meshy-pipeline]].

## Engine and tools
- Blender FBX export: use `FBX_SCALE_UNITS`; `FBX_SCALE_NONE` gives UnitScaleFactor 1 and a 100 times too large import. MCP `import_file` converts the scene unit; for an existing asset use [[tool-unreal-reimport]] with the editor closed.
- A WPO material that reads an extra UV channel: a static mesh with Generate Lightmap UVs on can overwrite that channel in the render data only. Read RENDER_DATA with Geometry Script (`copy_mesh_from_static_mesh`), not the source model, and check every triangle corner.
- Setting static mesh build settings headless needs `UnrealEditor-Cmd ... -ExecutePythonScript=<file>` (plain `-run=pythonscript` returns default structs and applies nothing).
- MCP: material toolset name is `editor_toolset.toolsets.material.MaterialTools` with short tool names; adding a Custom node input needs one `set_properties` call carrying all existing entries plus an empty `{}`, then a second call to name it; `get_expression_inputs` may misreport an input; asset tools (save, exists) fail while a PIE session runs: stop PIE first.
- Visual check without the editor: `UnrealEditor.exe <uproject> <map> -game -windowed -ExecCmds=...` with `rr.Debug.SpawnNear`, `rr.Debug.Shots`, `rr.Debug.Delayed`, `rr.Stress.Peace 1`, `getall StaticMeshComponent CustomPrimitiveDataInternal` reads the primitive data of every mesh.
- Owner videos (`.mkv` in `C:\Users\soumo\Videos`): extract frames with the WinGet ffmpeg (`ffmpeg -i f.mkv -vf "fps=4,scale=640:-1,tile=4x3" -frames:v 1 sheet.png`) and crop.
- `Tools/art/kie_image.js`: run id is the timestamp to the second, so parallel runs overwrite each other: run image generations one at a time.
- Meshy multi-view (docs): up to three extra views, same size, camera angle and framing, subject centred with margin; slots Front, Left, Back, Right; the docs give no angle: 90 degree profiles were used and worked; left/right is not defined in the docs.

## Process
- Permission classifier: a subagent's `taskkill` of the editor and my own `CaptureViewport` were denied. Close only an editor launched by the session, with `CloseMainWindow`; never work around a denial.
- The owner may close the editor or run PIE while an agent works: unsaved Blueprint edits are lost; check `IsPIERunning` and the process before delegating.
- Another session may stage vault files: commit by explicit path (`git commit -- <paths>`), never `git add vault`.
- `task-next.js` fails with "no usage" when a resumed agent's description does not start with the subtask id: close with `task-status.js` (subtask then global) and note that the cost is not recorded.
- The owner writes French: answer in French, never correct his spelling; he reports defects with screenshots and videos: describe what you see before theorising, and check with a real game run before deciding a cause.

## Relations
- [[agents-index]] — parent index.
- [[chaser-meshy-pipeline]] — the pipeline these lessons belong to.
- [[handover-2026-10-14-chaser-meshy]] — state and open items.
