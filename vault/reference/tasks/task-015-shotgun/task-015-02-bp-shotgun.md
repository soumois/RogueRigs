---
id: task-015-02-bp-shotgun
description: "White-gold conical wave VFX cut by rock, shotgun Blueprint and data, miner loadout, PIE check, docs."
parent: task-015-shotgun
kind: task
status: in-progress
---
# Shotgun Blueprint and VFX

- goal: the shotgun is visible and playable in PIE.
- agent: bp
- depends_on: task-015-01-shotgun-cpp
- inputs: [[shotgun]], [[color-code]], [[bp-miner]], [[bp-burst-rifle]], [[unreal-mcp]], [[cpp-bp-split]].
- expected_output: shotgun Blueprint (thin, like the rifle one) and a short white-gold cone flash using the C++ clipped length; miner loadout set; new doc `bp-shotgun`, [[bp-miner]] updated. PIE: both weapons fire, the wave pushes robots and stops at rock; no log error.
- done_when: PIE check with no error; docs updated; lint clean.
- editor_state: open
- token_ceiling: 90000
- milestone: M1
- result: open

## Relations
- [[task-015-shotgun]] — parent global task.
