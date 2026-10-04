---
id: task-015-02-bp-shotgun
description: "White-gold conical wave VFX cut by rock, shotgun Blueprint and data, miner loadout, PIE check, docs."
parent: task-015-shotgun
kind: task
status: done
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
- model: sonnet
- tokens: input 114, cache write 120567, cache read 4445407, output 31476
- runs: 2
- milestone: M1
- result: BP_Shotgun (child of ARRShotgun, cube mesh, Muzzle at 50 uu): OnWaveFired draws a fan of 7 white-gold rays (−30° to +30°) each cut at GetWaveClearDistance, destroyed after 0.15 s; BP_Miner StartingWeapons [BP_BurstRifle, BP_Shotgun]. PIE: both weapons kill robots (log), 13 waves in 16.5 s, LastWave range 700 clipped 700 in the open, hit count 0 then 1; no PIE log error. Not verified in PIE: knockback distance, rock clipping (covered by the C++ RockBlocks test), the flash on screen. MCP has no SetCVar tool, so rr.Cheat CVars cannot be set from MCP (docs to fix in task-016-01). Docs bp-shotgun (new), bp-miner, blueprints-index. Commits 2151727, 727cf23, 640c3c7; about 220k tokens over two runs.

## Relations
- [[task-015-shotgun]] — parent global task.
