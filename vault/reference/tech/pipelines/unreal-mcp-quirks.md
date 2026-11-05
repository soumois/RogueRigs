---
id: unreal-mcp-quirks
description: "Unreal MCP workarounds found in tasks 040–062: graph DSL, widgets, materials, Niagara, logs, paths. Open before a bp subtask that edits graphs, UMG or Niagara."
parent: unreal-mcp
kind: doc
---
# Unreal MCP quirks (tasks 040–062)

## Blueprint graphs
- `add_variable` fails when `container_type` is "None": omit the field.
- `write_graph_dsl`: pass `CallFunction` arguments by name (`:Param x`), never by position.
- Rewriting a graph with the DSL does not clear its old nodes: delete strays with `delete_node`, check with `read_graph_dsl`.
- An `AddEvent` can only bind to an event that already exists before the write; a custom event defined in the same write gets a suffixed name.
- Removing and re-adding a function graph may return the old name (`UpdateLocator_0`).
- `write_graph_dsl` does not unescape `
` in string literals: a pin default written as "
" holds two characters (backslash, n); set a real newline with `set_pin_value` (task-084-01). `set_pin_value` alone does not dirty the asset: also touch a widget property before `save_assets`, then check the file changed.
- Function ids that work: `Math|Trig|Sin(Degrees)`, `Widget|SetText(Text)`, `Class|Image|SetColorandOpacity`, `Class|Text|SetColorandOpacity`.

## Widgets and materials
- A widget's dynamic material: `Widget|Brush|GetDynamicMaterial` on the image brush, not `CreateDynamicMaterialInstance`; set its parameters with the `ByInfo` setters; set a texture with `SetBrushResourceObject`.
- A material Custom node's `Inputs` cannot be set in one call: set `[]`, then empty entries, then the names; a Saturate input is named "None".
- PIE widget instances have no object path reachable through MCP; use the Slate snapshot for visibility and geometry only.

## Objects and assets
- Class defaults: `/Game/.../BP_X.Default__BP_X_C`, not the `_C` class path.
- After editing a game mode Blueprint, compile and save it, or PIE keeps the old value.
- An asset cannot be re-imported over itself: import under a new name and switch the reference.
- MCP has no reimport (`StaticMeshTools.import_file` refuses an existing asset): with the editor closed, use [[tool-unreal-reimport]].

## Niagara
- `NiagaraToolsets.NiagaraToolset_System` can create a system from a template, add or remove emitters, renderers and modules, and set module inputs.
- An emitter without a spawn module spawns nothing while its component reports active (task-058).
- `SetStackInputData` replacing one HLSL expression by another changes the read-back but not the compiled script (compile state stays UpToDate, the game plays the old value; task-440-01, seen on a system copied from a template). Set a plain value first, then the expression, and check the value in the simulation.
- `AddUserVariables` takes `{name, type, description, defaultValue}` with `name` = `User.X`. A wrong field shape returns success and leaves an empty variable: the next `GetUserVariables` crashes the editor on `InCount > 0` in `NiagaraVariant.cpp`, unsaved work lost (task-441-04). Check with `GetSystemSummary` and save before calling it.
- Headless tests run with `-nullrhi`: Niagara never activates there (`FApp::CanEverRender()` is false); guard Niagara checks or run with `-RenderOffScreen`.
- A vector `max()` or `min()` in an HLSL SetParameters input compiled to ONE scalar used for all three axes (task-174: pieces spawned at (s,s,s) near the world origin). Split per component: `float3(v.x, v.y, max(v.z, limit))`, in a second SetParameters module if needed (AddSetParametersModule rejects an HLSL default: add with a plain default, then SetStackInputData).
- The engine Collision module (CPU Analytical Planes) from a template ships with `Analytical Plane Debug Mode` = 1: it draws a red plane (about 2 m) at the plane position in -game too. Set it to 0 (`SetStackInputData`, scriptName `ParticleUpdateScript`, moduleName `Collision`, NiagaraInt32) after adding a Collision module or cloning a template (task-179).
- GPU switch (task-191-01): `NiagaraToolset_System.SetEmitterData` with `propertyValues` JSON string `{"SimTarget":"GPUComputeSim","CalculateBoundsMode":"Fixed","FixedBounds":{"min":{...},"max":{...},"isValid":true}}` works; the system path must be the full object path `/Game/.../NS_X.NS_X` (a bare package path is rejected). The tool schema names differ per tool (`emitter`, `emitterRef`, `moduleRef`, `inputReference` vs `stackInputRef`). Collision module on GPU: input `GPU Collision Type` is a static switch (enum `/Niagara/Enums/ENiagara_GPUCollisionType`: NewEnumerator1 Depth Buffer, NewEnumerator2 Distance Fields; an unknown enumerator is silently ignored); only one mode at a time, so analytical planes and distance fields cannot be combined. `Collision Enabled` accepts an HLSL expression. `fx.Niagara.DumpComponents full` shows no GPU particle data.

## Logs and shell
- `LogsToolset.GetLogEntries` needs a valid category (LogBlueprint, LogScript, LogNiagara, LogRogueRigs).
- From Git Bash, set `MSYS_NO_PATHCONV=1` so `/Script/...` paths are not rewritten.

## Graph DSL (task-169-04, 2026-10-14)
- A multi-output `bind` on a Break node is positional: list all its output pin names.
- A positional `CallFunction` shifts its first argument into `self`: use named arguments.
- The `(|GetX)` form printed by read_graph_dsl cannot be written back: write `Variables|<Blueprint>|GetX`.
- A function parameter named like an input pin of a called node (WeaponName vs GetWeaponIconByName.WeaponName) collides: rename the parameter.
- `Appearance|SetBrushfromTexture` resolves to the Border variant: use `Appearance|SetBrushResourceObject`, then `SetDesiredSizeOverride` with the texture size to keep the aspect.
- Failed DSL writes leave unreachable node chains: find and delete orphans, then read the graph back before the final compile.

## Properties, textures and materials (2026-10-12 to 14)
- `set_properties` takes `values` as a JSON string; SizeBox overrides also need their `bOverride_*` flag.
- Pure nodes are re-evaluated at each consumer: a time delta computed after its setter reads zero.
- `TextureTools.import_file` refuses to overwrite: delete, import, then set the UI texture group again.
- Material graphs: `editor_toolset.toolsets.material.MaterialTools`, short tool names. A new Custom node input needs one `set_properties` call with all existing entries plus an empty `{}`, then a second call to name it; `get_expression_inputs` may misreport an input.
- Static mesh FBX import (task-350-02): `StaticMeshTools.import_file` gave a mesh 100x too small; editor Python (`py "<script>"` typed in the status-bar Cmd box through SlateInspectorToolset) with `FbxImportUI` + `AssetImportTask` imported at the right scale. A mesh first imported through MCP must be deleted before reimport; Python `static_materials` edits did not stick, `StaticMeshTools.set_material` per slot did.
- Static mesh FBX import, root cause and fix (task-400-02): `StaticMeshTools.import_file` gave a mesh 100x too small with only 1,130 of 9,892 triangles (degenerate removal). Fix: ObjectTools set `bConvertSceneUnit` true on `/Script/UnrealEd.Default__FbxStaticMeshImportData`, delete the asset, re-import, then reset it to false (`importUniformScale` 100 also works).

## Materials and BP components (task-403, 2026-10-31)
- `get_property_input` needs MP_ names (`MP_BaseColor`).
- `get_properties` refuses Coordinates, Source and CompressionNone.
- A BP component path needs the `_C` form: `BP_X_C:Comp_GEN_VARIABLE`.
- `LogsToolset.GetLogEntries` with a category filter errored; the compile log was not readable.
- A BP static mesh component can carry OverrideMaterials that hide the mesh material: read them first when a colour is wrong.

## Relations
- [[unreal-mcp]] — parent: the MCP setup and its general limits.
- [[tool-unreal-reimport]] — headless in-place reimport that MCP lacks.
