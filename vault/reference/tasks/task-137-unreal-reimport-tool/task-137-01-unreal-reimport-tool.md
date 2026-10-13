---
id: task-137-01-unreal-reimport-tool
description: "Create a headless Python tool that reimports a static mesh from its FBX in place, and document it in the vault."
parent: task-137-unreal-reimport-tool
kind: task
status: in-progress
---
# Tool: reimport a static mesh in place

- goal: Create a headless Python tool that reimports a static mesh from its FBX in place, and document it in the vault.
- agent: cpp
- depends_on: none
- inputs: build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), pipelines-index (vault/reference/tech/pipelines/pipelines-index.md)
- expected_output: Tools/unreal/reimport_static_mesh.py (unreal Python: for each given asset path, reimport it from the FBX stored in its AssetImportData with unreal.EditorReimportHandler.reimport; fallback unreal.AssetImportTask with replace_existing true, automated, save, import_materials false, combine_meshes true; then save the asset and print one line per asset: path, ok or error, bounds); a command line to run it headless with UnrealEditor-Cmd (-run=pythonscript -script="<file> <asset paths>" -unattended -nopause -nosplash, editor closed); a new doc node in tech/pipelines (id tool-unreal-reimport, kind doc, parent pipelines-index, with the command, the limits, the test result and the rule that the editor must be closed) listed in pipelines-index, and one pointer line in unreal-mcp-quirks (why MCP cannot reimport) and in blender-asset-route (reimport after a re-export).
- done_when: Test on SM_XPNut and SM_XPChipBig reimporting their unchanged FBX: the command exits 0, each asset path is unchanged with no duplicate, bounds and material slots identical before and after (nut z -5.85 to 5.85, slots M_XPNut_body and M_XPNut_hole; chip z -5.3 to 5.3, seven M_XPChipBig_* slots), the Blueprint overrides of BP_PickupXP and BP_PickupXPBig keep their instances; a failing path gives a non-zero exit code and a clear error; no other asset modified; lint clean; committed
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-137-unreal-reimport-tool]] — parent global task.
