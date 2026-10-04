---
id: task-014-04-bp-upgrade-ui
description: "Level-up ring effect, 3-card upgrade widget with rarity colors and one reroll button, spawner values on the map, PIE check."
parent: task-014-upgrades-levelup
kind: task
status: done
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
- model: sonnet
- tokens: input 266, cache write 241597, cache read 19228948, output 80733
- runs: 4
- milestone: M1
- result: WBP_HUD: UpgradeScreen (dim backdrop, title Choisis une amélioration, 3 tier-colored cards name/target/value, Relancer button) bound to OnUpgradeOfferShown/Closed, ChooseUpgrade(0..2), RequestReroll (no separate WBP_UpgradeChoice: cross-Blueprint calls not creatable via MCP). BP_Miner: cyan disc grows 0.6 s on OnLevelUpEffect. BP_EnemySpawner weights 85/15. PIE: pause 0.8 s after level-up with frozen frame, reroll once then disabled, card PV max +15 applied and resume — pass; ring seen; heal 20 % not isolated in PIE (C++ tests cover it); clicks driven by Slate Click + Enter (real mouse click unconfirmed). Cosmetic: card widths differ at 1920 px. No PIE runtime error. Docs wbp-hud, bp-miner, bp-enemy-spawner. Commits a6a715e, abc4ea7, df5a4fb; about 245k tokens over four agent runs (turn limit hit three times, screenshots overflowed context). Editor force-closed with a Save Content prompt pending (screen locked); only uncommitted in-memory edits lost.

## Relations
- [[task-014-upgrades-levelup]] — parent global task.
