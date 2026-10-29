# Unreal MCP tool sheet (generated)

Generated 2026-10-29 by `node Tools/unreal/mcp-sheet.js` from 14 bp runs of tasks 332-357; do not edit by hand, rerun after an engine or plugin update.
Call: `mcp__unreal__call_tool` with `toolset_name` (full name below), `tool_name` (bare name), `arguments` (object). A `ref(X)` argument is `{"refPath": "/Game/Path/Asset.Asset"}`; a widget is `<WBP refPath>:WidgetTree.<Name>`, a slot `<widget refPath>.<SlotName>`. `values` of set_properties is a JSON string.
Format: `tool(required: type, ...) [optional, ...] xN` (N = calls in those runs). Use describe_toolset only for a tool missing here.

## editor_toolset.toolsets.asset.AssetTools
- save_assets(asset_paths: string[]) x21

## editor_toolset.toolsets.blueprint.BlueprintTools
- add_function_graph(blueprint: ref(Blueprint), graph_name: string) x17
- add_function_param(graph: ref(EdGraph), param_name: string, param_type: string, input_param: boolean) [container_type] x10
- add_object_function_param(graph: ref(EdGraph), param_name: string, object_class: ref(Class), input_param: boolean) [container_type] x4
- add_object_variable(blueprint: ref(Blueprint), name: string, object_class: ref(Class)) [graph, container_type] x3
- add_struct_function_param(graph: ref(EdGraph), param_name: string, struct_type: ref(ScriptStruct), input_param: boolean) [container_type] x1
- add_variable(blueprint: ref(Blueprint), name: string, type_name: string) [graph, container_type] x13
- break_pins(output_pin: {direction,index_id,node}, input_pin: {direction,index_id,node}) x2
- compile_blueprint(blueprint: ref(Blueprint)) [warnings_as_errors] x13
- connect_pins(output_pin: {direction,index_id,node}, input_pin: {direction,index_id,node}) x36
- create_node(graph: ref(EdGraph), type_id: string, pos: {x,y}) [declaring_class] x19
- delete_node(node: ref(EdGraphNode)) x18
- find_node_types(graph: ref(EdGraph), type_id_filter: string, context_pins: {direction,index_id,node}[]) x46
- find_nodes(graph: ref(EdGraph), title: string) [node_class, entry_points_only] x46
- get_connected_subgraph(node: ref(EdGraphNode)) x7
- get_graph(blueprint: ref(Blueprint), graph_name: string) x4
- get_graph_dsl_docs() x8
- get_node_infos(nodes: ref(EdGraphNode)[]) x20
- get_node_type_pins(graph: ref(EdGraph), type_id: string) x13
- list_functions(blueprint: ref(Blueprint)) x1
- list_graphs(blueprint: ref(Blueprint)) x10
- list_variables(blueprint: ref(Blueprint)) [graph] x4
- read_graph_dsl(graph: ref(EdGraph)) x65
- remove_function_graph(blueprint: ref(Blueprint), graph_name: string) x1
- set_pin_value(pin: {direction,index_id,node}, value: string) x3
- write_graph_dsl(graph: ref(EdGraph), code: string) x46

## editor_toolset.toolsets.object.ObjectTools
- get_properties(instance: ref(Object), properties: string[]) x61
- list_properties(instance: ref(Object)) x18
- set_properties(instance: ref(Object), values: string) x172

## editor_toolset.toolsets.texture.TextureTools
- get_size(texture: ref(Texture2D)) x10
- import_file(folder_path: string, asset_name: string, source_file: string) x10

## EditorToolset.EditorAppToolset
- IsPIERunning() x14

## EditorToolset.LogsToolset
- GetLogEntries(pattern: string) [category, maxEntries] x2

## UMGToolSet.UMGToolSet
- AddWidget(widgetBlueprint: ref(WidgetBlueprint), widgetClass: ref(class:Widget), widgetDisplayName: string) [parentWidget, childIndex] x41
- CompileWidgetBlueprint(widgetBlueprint: ref(WidgetBlueprint)) x16
- GetWidgetDescription(widgetBlueprint: ref(WidgetBlueprint)) [startWidget, maxDepth] x28
- GetWidgets(widgetBlueprint: ref(WidgetBlueprint)) x8
- MoveWidget(widgetBlueprint: ref(WidgetBlueprint), widget: ref(Widget), newParent: ref(PanelWidget)) [childIndex] x16
- RemoveWidget(widgetBlueprint: ref(WidgetBlueprint), widget: ref(Widget)) x3
- RenameWidget(widgetBlueprint: ref(WidgetBlueprint), widget: ref(Widget), newDisplayName: string) x5
- ReplaceWidgetWithChild(widgetBlueprint: ref(WidgetBlueprint), widgetToReplace: ref(Widget)) x1
- ToggleWidgetAsVariable(widgetBlueprint: ref(WidgetBlueprint), widget: ref(Widget), bIsVariable: boolean) x5
- WrapWidgets(widgetBlueprint: ref(WidgetBlueprint), widgets: ref(Widget)[], wrapperClass: ref(class:PanelWidget)) x4

Never succeeded in those runs (wrong name, do not call): `ProgrammaticToolset.list_tools`, `AssetTools.list_tools`, `TextureTools.editor_toolset.toolsets.texture.TextureTools.import_file`, `BlueprintTools.list_nodes`.
