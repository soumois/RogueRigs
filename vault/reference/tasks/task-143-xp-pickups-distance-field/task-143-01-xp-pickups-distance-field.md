---
id: task-143-01-xp-pickups-distance-field
description: "Turn on the mesh distance field of the XP nut and chip and test whether they light the floor."
parent: task-143-xp-pickups-distance-field
kind: task
status: todo
---
# XP pickups: mesh distance field test

- goal: Turn on the mesh distance field of the XP nut and chip and test whether they light the floor.
- agent: bp
- depends_on: none
- inputs: bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md), color-code (vault/reference/gdd/art-audio/color-code.md)
- expected_output: On /Game/RogueRigs/Art/XPNut/SM_XPNut and /Game/RogueRigs/Art/XPChip/SM_XPChipBig set bGenerateMeshDistanceField true through ObjectTools (read back), then make the mesh rebuild its distance field (saving the asset or a build call the tools offer; if the tools offer no rebuild, report it: the owner can press Apply Changes in the mesh editor), and save the two meshes only. Read-only: report the project rendering settings that matter for Lumen software tracing (Generate Mesh Distance Fields, Dynamic Global Illumination Method, Software Ray Tracing Mode / hardware ray tracing) with the tools you have (ConfigSettingsToolset or ObjectTools on the project settings object) and do NOT change any project or global setting. If a property cannot be set or a save fails, STOP and report it: do not use SlateInspectorToolset, key presses or any UI automation, and do not save anything that is not named here (the editor is the owner's and may hold his unsaved edits). Update the bp-pickup-xp doc node: the flag set, provisional test, to be judged by the owner in the viewport (no screenshots by the agent).
- done_when: bGenerateMeshDistanceField reads true on both meshes, both saved and modified in git, project settings reported, nothing else changed, doc updated, no UI automation used
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-143-xp-pickups-distance-field]] — parent global task.
