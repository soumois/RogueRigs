---
id: task-014-04-bp-upgrade-ui
description: "Level-up ring effect, 3-card upgrade widget with rarity colors and one reroll button, spawner values on the map, PIE check."
parent: task-014-upgrades-levelup
kind: task
status: todo
---
# Upgrade UI and level-up effect

- goal: the player sees the effect and picks a card with the mouse.
- agent: bp
- depends_on: task-014-03-levelup-flow
- inputs: [[xp-level-up]], [[rarities]], [[color-code]], [[readability-rules]], [[wbp-hud]], [[bp-miner]], [[bp-enemy-spawner]], [[unreal-mcp]], [[cpp-bp-split]].
- expected_output: pulsing cyan ring on the miner at level-up (simple Niagara or mesh scale fade); `WBP_UpgradeChoice`: 3 cards (name, value text, border in tier color grey, green, blue, purple, yellow), click picks, a "Relancer" button disabled after one use; shown on the C++ offer event; `BP_EnemySpawner` defaults `WeightedEnemyClasses` set to BP_Chaser 85 / BP_Kamikaze 15 (cap 100 and packs come from C++ defaults; the placed instance overrides nothing). Blueprint docs created or updated ([[wbp-hud]], [[bp-miner]], [[bp-enemy-spawner]], new widget doc). PIE: reach a level-up, see ring and heal, pause, cards, reroll once, pick, resume; no log error.
- done_when: PIE check done with no error in the log; Blueprint docs updated; lint clean.
- editor_state: open
- token_ceiling: 110000
- milestone: M1
- result: open

## Relations
- [[task-014-upgrades-levelup]] — parent global task.
