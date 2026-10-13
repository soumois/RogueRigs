---
id: task-118-01-telegraph-fill
description: "The ground telegraph draws the growing circle as a transparent filled disc (inside of the zone transparent) for console, chest, barrel and heavy."
parent: task-118-telegraph-fill
kind: task
status: done
---
# Ground telegraph fill

- goal: The ground telegraph draws the growing circle as a transparent filled disc (inside of the zone transparent) for console, chest, barrel and heavy.
- agent: bp
- depends_on: none
- inputs: ground-telegraph (vault/reference/gdd/art-audio/ground-telegraph.md), sys-ground-telegraph (vault/reference/tech/systems/combat/sys-ground-telegraph.md), objective-console (vault/reference/gdd/run/objective-console.md), enemy-heavy (vault/reference/gdd/enemies/enemy-heavy.md), barrel (vault/reference/gdd/terrain/barrel.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), sys-tests-explosions-barrel (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-explosions-barrel.md)
- expected_output: M_GroundTelegraph (or its instances) draws the progress disc filled with a transparent colour per mode (white-blue hacks, orange danger), zone interior transparent, solid edge kept; sys-ground-telegraph updated.
- done_when: material compiles with no error in the editor log; parameters read back (fill opacity per mode); sys-ground-telegraph and ground-telegraph docs match; lint clean; no C++ change unless a parameter is missing (report it instead)
- editor_state: editor open (Unreal MCP); material built through MaterialTools; no screenshots by the agent
- token_ceiling: 40 turns, commit after each group
- model: sonnet
- tokens: input 44, cache write 49760, cache read 701643, output 11580
- runs: 1
- milestone: M2
- result: M_GroundTelegraph: filled progress disc, FillOpacity 0.25, shader recompiled with no error, read back; no instances to set; commit 3715add

## Relations
- [[task-118-telegraph-fill]] — parent global task.
