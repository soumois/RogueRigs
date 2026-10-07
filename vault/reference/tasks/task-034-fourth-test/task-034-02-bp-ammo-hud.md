---
id: task-034-02-bp-ammo-hud
description: "WBP_HUD weapon slots: ammo bar above 7 rounds, blocks at 7 or fewer, red reload bar, turret charge blocks with recharge block, padlocks."
parent: task-034-fourth-test
kind: task
status: todo
---
# Fourth test: ammo HUD

- goal: each weapon slot shows its ammo and reload state; empty slots look locked.
- agent: bp
- depends_on: task-034-01-cpp-fixes
- inputs: [[wbp-hud]], [[sys-hud]], [[readability-rules]], [[color-code]], [[unreal-mcp]], [[sys-cheats]]
- expected_output: WBP_HUD, under each owned weapon slot: above 7 rounds a bar of rounds left / magazine size; at 7 or fewer one block per round (filled = loaded); while reloading a red bar filling with the reload progress; turret launcher: one block per charge, the recharging charge shown as a red bar in the leftmost empty block (it moves left when another charge is used); slots not owned show a padlock (no level text). Driven by the task-034-01 getters. Docs wbp-hud. PIE: no log error; read back the bar, the block count and the reload bar during a reload.
- done_when: PIE check with no error; docs updated; lint clean.
- editor_state: open
- token_ceiling: 110000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-034-fourth-test]] — parent global task.
