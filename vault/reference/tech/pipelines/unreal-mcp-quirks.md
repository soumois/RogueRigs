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
- Headless tests run with `-nullrhi`: Niagara never activates there (`FApp::CanEverRender()` is false); guard Niagara checks or run with `-RenderOffScreen`.

## Logs and shell
- `LogsToolset.GetLogEntries` needs a valid category (LogBlueprint, LogScript, LogNiagara, LogRogueRigs).
- From Git Bash, set `MSYS_NO_PATHCONV=1` so `/Script/...` paths are not rewritten.

## Relations
- [[unreal-mcp]] — parent: the MCP setup and its general limits.
- [[tool-unreal-reimport]] — headless in-place reimport that MCP lacks.
