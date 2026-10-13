---
id: task-137-unreal-reimport-tool
description: "Global task: Create a headless Python tool that reimports a static mesh from its FBX in place, and document it in the vault."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Tool: reimport a static mesh in place

- request: Create a tool so a mesh can be reimported in place when its FBX changes, and document it in the vault; the MCP import_file refuses an existing asset and the Python Editor Script plugin is enabled (owner, 2026-10-13) (owner, 2026-10-13)
- goal: Create a headless Python tool that reimports a static mesh from its FBX in place, and document it in the vault.
- acceptance: Test on SM_XPNut and SM_XPChipBig reimporting their unchanged FBX: the command exits 0, each asset path is unchanged with no duplicate, bounds and material slots identical before and after (nut z -5.85 to 5.85, slots M_XPNut_body and M_XPNut_hole; chip z -5.3 to 5.3, seven M_XPChipBig_* slots), the Blueprint overrides of BP_PickupXP and BP_PickupXPBig keep their instances; a failing path gives a non-zero exit code and a clear error; no other asset modified; lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: task-137-01-unreal-reimport-tool
- result: open

## Subtasks
1. task-137-01-unreal-reimport-tool — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-137-01-unreal-reimport-tool]] — step 1, cpp.
