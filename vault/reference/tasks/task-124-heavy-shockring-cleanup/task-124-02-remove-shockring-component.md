---
id: task-124-02-remove-shockring-component
description: "Delete the hidden ShockRing component from BP_Heavy after the C++ test no longer looks it up."
parent: task-124-heavy-shockring-cleanup
kind: task
status: in-progress
---
# Remove the ShockRing component from BP_Heavy

- goal: BP_Heavy has no ShockRing component and still compiles and works (glow, tilt, ground telegraph unchanged).
- agent: bp
- depends_on: task-124-01-heavy-shockring-cleanup
- inputs: bp-heavy (vault/reference/tech/blueprints/enemies/bp-heavy.md), sys-enemies-heavy (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-heavy.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: ShockRing component deleted from BP_Heavy, Blueprint compiled and saved, bp-heavy doc updated.
- done_when: component list read back without ShockRing; compile and save succeed; bp-heavy doc matches; lint clean
- editor_state: editor open (Unreal MCP); no screenshots by the agent
- token_ceiling: 40 turns, commit after each group
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: ShockRing component deleted from BP_Heavy; no node or variable referenced it; compiled and saved; bp-heavy and sys-enemies-heavy updated.

## Relations
- [[task-124-heavy-shockring-cleanup]] — parent global task.
