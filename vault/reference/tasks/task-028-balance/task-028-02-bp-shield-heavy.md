---
id: task-028-02-bp-shield-heavy
description: "BP_ShieldChaser with a pale blue-white translucent shell that breaks, BP_Heavy ring at 400 uu, spawner weights, PIE."
parent: task-028-balance
kind: task
status: todo
---
# Shield chaser and heavy visuals

- goal: the new enemy and radius are visible.
- agent: bp
- depends_on: task-028-01-heavy-shield-values
- inputs: [[bp-chaser]], [[bp-heavy]], [[bp-enemy-spawner]], task-028-01 result, [[unreal-mcp]], [[sys-cheats]], [[cpp-bp-split]], [[color-code]], [[readability-rules]], [[drgs-analysis]]
- expected_output: BP_ShieldChaser (BP_Chaser look + translucent pale blue-white shell, hidden on OnShieldBroken with a short burst); BP_Heavy telegraph shows the slam area on the ground during the telegraph (ring at the real radius); spawner data for shield chasers; docs. PIE check.
- done_when: PIE check through the cheat property route with no log error; Blueprint docs updated; lint clean.
- editor_state: open
- token_ceiling: 100000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-028-balance]] — parent global task.
