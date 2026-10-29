---
id: unreal-mcp
description: "Epic's Unreal MCP plugin (UE 5.8, experimental): how to start it, how Claude Code connects, known toolsets, limits, and the tool inventory status."
parent: pipelines-index
kind: doc
---
# Unreal MCP

- Plugins enabled in the project: **Unreal MCP** (Epic, experimental) and **All Toolsets** (aggregator of every toolset plugin).
- The server runs inside the editor process: the editor must be open ([[editor-workflow]]).
- Start: Editor Preferences → Model Context Protocol → Auto Start Server, or the console command `ModelContextProtocol.StartServer`.
- Endpoint: `http://127.0.0.1:8000/mcp` (HTTP with Server-Sent Events; port and path set in Editor Preferences). Loopback only, no authentication.
- Claude Code connects through the project file `.mcp.json` (server name `unreal`).

## Known toolsets
Named in Epic's documentation: `ActorTools`, `SceneTools`, `MaterialInstanceTools`, `ObjectTools`. Exposed functionality includes spawning actors, lighting, material instances, inspecting Slate widgets, running automation tests.

## Tool inventory
- Generated sheet of the 43 tools bp agents use, with argument shapes: `Tools/unreal/mcp-tool-sheet.md`; regenerate with `node Tools/unreal/mcp-sheet.js` after an engine or plugin update ([[bp-mcp-lessons]]).
- Recorded 2026-10-01 (task-001-04). Access: `list_toolsets`, `describe_toolset`, then `call_tool` with `toolset_name` = full toolset name and `tool_name` = bare function name (for example `create_material`, not the dotted full name). `ObjectTools.set_properties` takes `values` as a JSON string.
- Asset and editor toolsets (`editor_toolset.toolsets.*`): `actor.ActorTools`, `asset.AssetTools` (find, load, save_assets, move, duplicate, delete, create_folder, tags, referencers), `blueprint.BlueprintTools`, `material.MaterialTools` (create_material, add_expression, connect_to_output, recompile), `material_instance.MaterialInstanceTools` (create, set_vector_parameter, set_scalar_parameter), `object.ObjectTools` (list/get/set_properties, search_subclasses), `scene.SceneTools`, `primitive.PrimitiveTools`, `static_mesh.StaticMeshTools`, `skeletal_mesh.SkeletalMeshTools`, `texture.TextureTools`, `data_asset.DataAssetTools`, `data_table.DataTableTools`, `curve_table.CurveTableTools`, `string_table.StringTableTools`, `programmatic.ProgrammaticToolset` (sandboxed Python script batching other tools).
- Other toolsets: `EditorToolset.EditorAppToolset` (selection, viewport, PIE; no console variable or console command tool in this editor build, reported by the BP agent before task-016-01), `EditorToolset.LogsToolset`, `UMGToolSet.UMGToolSet`, `NiagaraToolsets.*` (Info, Component, Blueprint, System, Assets), `PCGToolset.*`, `GASToolsets.*`, `GameplayTagsToolset`, `GameFeaturesToolset`, `ConfigSettingsToolset`, `AutomationTestToolset`, `SemanticSearchToolset`, `DataRegistryToolset`, `PluginToolset`, `PhysicsToolsets.PhysicsAssetToolset`, `SlateInspectorToolset`, `state_tree_toolset`, `aimodule_toolset` (behavior tree), `conversation_toolset`, `animation_toolset.*` (Sequencer, Control Rig), `ToolsetRegistry.AgentSkillToolset`, `DataflowAgent`, `WorldConditionsToolset`.
- Verified in practice: base material with Vector and Scalar parameters, material instances with parameter overrides, saving assets.
- Blueprint creation and editing through MCP works: the owner used it on another project to build a Blueprint with its rendering, materials and collision script.
- Quirks (task-001-05/06): `ObjectTools` args use `instance` (not `object`); `AssetTools.save_assets` takes `asset_paths` as plain content paths without `.Name`; a HISM `InstanceCount` is unreadable, count `xPlane` entries in `PerInstanceSMData`. No tool creates, saves-as or discards a level, and none calls a UFunction (`GetSpawnCell` not callable): duplicate a level with `AssetTools.duplicate`, then `SceneTools.load_level` and `remove_from_scene`. `load_level` refuses while the current level is dirty; File > New Level through `SlateInspectorToolset` (Click) replaced the dirty level without a prompt. PIE: `StartPIE`, `find_actors` on the `UEDPIE_` world, `get_actor_transform`, `StopPIE`.
- Quirks (task-002-02): `execute_tool_script` needs a `run()` returning a dict with string keys. `SceneTools.find_actors` requires `name`, `tag`, `collision_channels` even when empty. `get_referencers` lists external-actor packages. `AssetTools.delete` on a content folder leaves `__ExternalActors__`/`__ExternalObjects__` (delete them in a second call). No fixup-redirectors tool: re-save referencers, or clear and reset a property to dirty them.
- Quirks (task-002-05): `ObjectTools.get_properties` takes `properties` (list), not `property_names`. `LogsToolset.GetLogEntries` needs `pattern`. `SlateInspectorToolset.PressKey` is press+release in one event: no held key, the PIE pawn did not move (movement cannot be verified this way).

- Quirks (task-004-04): no tool creates Enhanced Input assets (duplicate existing ones). `set_properties` on an array that changes size and elements at once fails: set it to `[]` first. An instanced subobject is created with `{"refPath": "/Script/Module.Class"}` (the `{"instance": ...}` form gives None). PIE subobject paths use `.` (e.g. `BP_Miner_C_0.CharMoveComp`). `compile_blueprint` returns null on success.
- Quirks (task-007-04/008-03): `StartPIE` options = {"bSimulate":false,"playMode":"PlayMode_InViewPort","warmupSeconds":3}. `SlateInspectorToolset.PressKey` takes a chord as one string ("Ctrl+Shift+S"). `ActorTools.save_actor` fails on new external actors; `AssetTools.save_assets` on the map returns true but does not write moved external actors: press Ctrl+Shift+S, then check `git status` for `__ExternalActors__` changes. `ActorTools.set_actor_transform` takes `xform` with a `location` only (unset fields unchanged). A BP component added in the SCS (e.g. `RifleMesh`) is not readable through the CDO path.
- Quirks (task-005-02): object refs need the full `Pkg.Name` form (`/Game/.../M_GreyBox.M_GreyBox`; the bare package path is rejected). `BlueprintTools.create` takes `asset_type` = the parent class ref. A C++ inherited component is edited on the CDO with `get_default_object` then `set_properties` on `<CDO path>:Mesh`. `StartPIE` needs `options` (`bSimulate`, `playMode`, `warmupSeconds`). `ActorTools.get_actor_transform` reads a PIE actor.

## Driving PIE checks
- MCP cannot set console variables, run console commands or call a UFunction, so the `rr.Cheat.*` variables are out of reach. It can set UObject properties, including on a class default object: during PIE call `ObjectTools.set_properties` with `instance` = `/Script/RogueRigs.Default__RRCheatSettings` and `values` as a JSON string, e.g. `{"GrantXP": 5}` (level 1 → 2, card screen 0.8 s later), `{"bReroll": true}`, `{"ChooseUpgrade": 1}`, `{"bInvulnerable": true}`, `{"AlertSpeedMultiplier": 20}`, `{"bSkipToConsole": true}`, `{"bCompleteConsoleHack": true}`, `{"bSkipToExtraction": true}` ([[sys-cheats]]). One-shot fields are consumed and reset by the next frame (read them back with `get_properties`); each action logs `rr.Cheat.<Name> (RRCheatSettings)` (read with `LogsToolset.GetLogEntries`, pattern `rr.Cheat`). Reset `bInvulnerable` and `AlertSpeedMultiplier` after the check: the CDO outlives PIE.

## Limits
- Experimental: features incomplete, APIs and formats may change.
- Each session starts cold: the agent discovers the project one tool call at a time, so subtasks must name the exact assets and classes involved.
- New tools need an editor restart (Live Coding does not propagate them).

Source: https://dev.epicgames.com/documentation/unreal-engine/unreal-mcp-in-unreal-editor

## Relations
- [[unreal-mcp-epic-plugin]] — rules and tools taken from Epic's Claude Code plugin (not installed); read before a bp subtask or a custom toolset.
- [[unreal-mcp-quirks]] — workarounds for graphs, widgets, materials, Niagara and logs found in tasks 040–062; read before a bp subtask.
- [[pipelines-index]] — parent index.
- [[agent-bp]] — the agent that uses it.
- [[sys-cheats]] — cheat properties MCP sets on the CDO to drive PIE checks.
