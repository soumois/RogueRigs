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
- Recorded 2026-10-01 (task-001-04). Access: `list_toolsets`, `describe_toolset`, then `call_tool` with `toolset_name` = full toolset name and `tool_name` = bare function name (for example `create_material`, not the dotted full name). `ObjectTools.set_properties` takes `values` as a JSON string.
- Asset and editor toolsets (`editor_toolset.toolsets.*`): `actor.ActorTools`, `asset.AssetTools` (find, load, save_assets, move, duplicate, delete, create_folder, tags, referencers), `blueprint.BlueprintTools`, `material.MaterialTools` (create_material, add_expression, connect_to_output, recompile), `material_instance.MaterialInstanceTools` (create, set_vector_parameter, set_scalar_parameter), `object.ObjectTools` (list/get/set_properties, search_subclasses), `scene.SceneTools`, `primitive.PrimitiveTools`, `static_mesh.StaticMeshTools`, `skeletal_mesh.SkeletalMeshTools`, `texture.TextureTools`, `data_asset.DataAssetTools`, `data_table.DataTableTools`, `curve_table.CurveTableTools`, `string_table.StringTableTools`, `programmatic.ProgrammaticToolset` (sandboxed Python script batching other tools).
- Other toolsets: `EditorToolset.EditorAppToolset` (console variables, selection, viewport, PIE), `EditorToolset.LogsToolset`, `UMGToolSet.UMGToolSet`, `NiagaraToolsets.*` (Info, Component, Blueprint, System, Assets), `PCGToolset.*`, `GASToolsets.*`, `GameplayTagsToolset`, `GameFeaturesToolset`, `ConfigSettingsToolset`, `AutomationTestToolset`, `SemanticSearchToolset`, `DataRegistryToolset`, `PluginToolset`, `PhysicsToolsets.PhysicsAssetToolset`, `SlateInspectorToolset`, `state_tree_toolset`, `aimodule_toolset` (behavior tree), `conversation_toolset`, `animation_toolset.*` (Sequencer, Control Rig), `ToolsetRegistry.AgentSkillToolset`, `DataflowAgent`, `WorldConditionsToolset`.
- Verified in practice: base material with Vector and Scalar parameters, material instances with parameter overrides, saving assets.
- Blueprint creation and editing through MCP works: the owner used it on another project to build a Blueprint with its rendering, materials and collision script.
- Quirks (task-001-05/06): `ObjectTools` args use `instance` (not `object`); `AssetTools.save_assets` takes `asset_paths` as plain content paths without `.Name`; a HISM `InstanceCount` is unreadable, count `xPlane` entries in `PerInstanceSMData`. No tool creates, saves-as or discards a level, and none calls a UFunction (`GetSpawnCell` not callable): duplicate a level with `AssetTools.duplicate`, then `SceneTools.load_level` and `remove_from_scene`. `load_level` refuses while the current level is dirty; File > New Level through `SlateInspectorToolset` (Click) replaced the dirty level without a prompt. PIE: `StartPIE`, `find_actors` on the `UEDPIE_` world, `get_actor_transform`, `StopPIE`.

## Limits
- Experimental: features incomplete, APIs and formats may change.
- Each session starts cold: the agent discovers the project one tool call at a time, so subtasks must name the exact assets and classes involved.
- New tools need an editor restart (Live Coding does not propagate them).

Source: https://dev.epicgames.com/documentation/unreal-engine/unreal-mcp-in-unreal-editor

## Relations
- [[pipelines-index]] — parent index.
- [[agent-bp]] — the agent that uses it.
