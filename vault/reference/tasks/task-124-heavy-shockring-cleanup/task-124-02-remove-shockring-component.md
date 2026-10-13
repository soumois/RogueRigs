---
id: task-124-02-remove-shockring-component
description: "Delete the hidden ShockRing component from BP_Heavy after the C++ test no longer looks it up."
parent: task-124-heavy-shockring-cleanup
kind: task
status: done
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
- model: sonnet
- tokens: input 36, cache write 34807, cache read 391936, output 7302
- runs: 1
- milestone: M2
- result: ShockRing component removed from BP_Heavy, compiled and saved, read back; commit 36483d2

## Relations
- [[task-124-heavy-shockring-cleanup]] — parent global task.
